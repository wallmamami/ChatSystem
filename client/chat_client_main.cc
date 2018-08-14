#include <iostream>
#include <signal.h>
#include "../server/api.hpp"
#include "client.h"
#include "window.h"

client::ChatClient* g_client = NULL;
client::Window* g_window = NULL;

void* Send(void *arg)
{
    // a）发送线程：读取输入数据，发送给服务器
    (void)arg;
    while(1)
    {
        std::string msg;
        g_window->DrawInput();
        g_window->GetStrFronWin(g_window->input_win_, &msg);
        g_client->SendMsg(msg);
    }

    return NULL;
}

void* Recv(void *arg)
{
    // b）接收线程：从服务器中读取数据，显示到界面上
    (void)arg;
    while(1)
    {
        //次此处需要绘制连个窗口，因为收到一条消息后，output
        //窗口需要更新，好友列表窗口可能也需要更新
        g_window->DrawOutput();
        g_window->DrawFriendList();
        server::Data data;
        g_client->RecvMsg(&data);
        if(data.cmd == "quit")
        {
            //删除的方式和插入的方式能够对应上
            g_window->DelFriend(data.name + "|" + data.school);
        }
        else
        {
            //好友列表的格式如下
            //王佳|陕科大
            //王佳2|西工院
            g_window->AddFriend(data.name + "|" + data.school);
            g_window->AddMsg(data.name + ":" + data.msg);
        }
    }
    return NULL;
}

void Quit(int sig)
{
    (void) sig;
    delete g_window;//最重要是为了调用它的析构函数，因为这个函数对ncurses进行了销毁动作，防止终端混乱
    delete g_client;

    exit(0);
}
//google 常见的C++开源库
//protobuf：用来序列化
//glog：打日志
//gflag：配置项管理
//grpc：RPC（封装socket库， 也是一种协议）框架
//gtest：单元测试框架
void Run(const std::string& ip, short port)
{
    //0.捕捉SIGINT信号
    signal(SIGINT, Quit);
    //1.初始化客户端核心模块
    g_client = new client::ChatClient();
    int ret = g_client->Init(ip, port);
    if(ret < 0)
    {
        printf("client Init failed!\n");
        return;
    }
    //2.提示用户输入用户名和学校
    std::string name, school;
    std::cout << "请输入用户名：" <<std::endl;
    std::cin >> name;
    std::cout << "请输入学校：" << std::endl;
    std::cin >> school;
    g_client->SetUserInfo(name, school);
    //3.初始化用户界面模块
    g_window = new client::Window();
    g_window->DrawHeader();
    //4.创建2个线程
    pthread_t stid, rtid;
    // a）发送线程：读取输入数据，发送给服务器
    pthread_create(&stid, NULL, Send, NULL);
    // b）接收线程：从服务器中读取数据，显示到界面上
    pthread_create(&rtid, NULL, Recv, NULL);
    pthread_join(stid, NULL);
    pthread_join(rtid, NULL);
    //5.对进程退出时进行处理
    delete g_window;
    delete g_client;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./chat_client [ip port]\n");
        return 1;
    }

    Run(argv[1], atoi(argv[2]));
    return 0;
}
