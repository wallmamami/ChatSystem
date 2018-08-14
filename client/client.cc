#include "client.h"
#include <iostream>
namespace client
{

//ip和port是通过命令行参数来传的
//可以改进成从配置文件中读取ip和port，可以使用json组织json
int ChatClient::Init(const std::string& server_ip, short server_port)
{
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_ < 0)
    {
        perror("socket");
        return -1;
    }
    server_addr_.sin_family = AF_INET;

    //C++ 11 中c_str() data()等价
    server_addr_.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_addr_.sin_port = htons(server_port);

    return 0;
}

int ChatClient::SetUserInfo(const std::string& name, const std::string& school)
{
    name_ = name;
    school_ = school;
    return 0;
}

void ChatClient::SendMsg(const std::string& msg)
{
    //下面这种发送方式的错误的
    //sendto(sock_, msg.c_str(), msg.size(), 0, (sockaddr*)&server_addr_, sizeof(server_addr_));
    

    //数据准备工作
    server::Data data;
    data.name = name_;
    data.school = school_;
    data.msg = msg;

    //下面表示约定了什么样的内容用户下线
    if(data.msg == "quit" || data.msg == "exit" || data.msg == "Q")
    {
        data.cmd = "quit";
    }

    std::string str;
    data.Serialize(&str);

    //真正发送数据--经过Json序列化后的数据
    sendto(sock_, str.c_str(), str.size(), 0, (sockaddr*)&server_addr_, sizeof(server_addr_));
    return;
}

void ChatClient::RecvMsg(server::Data* data)
{
    char buf[1024 * 10] = {0};

    //由于对端的ip端口号就是服务器的，所以此处没有必要再获取一遍
    ssize_t s = recvfrom(sock_, buf, sizeof(buf)-1, 0, NULL, NULL);
    if(s < 0)
    {
        perror("recvfrom");
        return;
    }
    buf[s] = '\0';
    data->UnSerialize(buf);

    return;
}

} //end client


#ifdef TEST_CHAT_CLIENT
void* Send(void* arg)
{
    //循环发送数据
    client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
    while(true)
    {   
        //std::cout << "请输入消息：";
        std::string str;
        std::cin >> str;

        client->SendMsg(str);
    }
    return NULL;
}

void* Recv(void* arg)
{
    //循环接收数据
    client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
    while(true)
    {
        server::Data data;
        client->RecvMsg(&data);

        //[wangjia|shankeda] haha
        std::cout << "[" << data.name << "|" << data.school << "]" << data.msg << "\n";
    }
    return NULL;
}

//简易版客户端，用来测试
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s [ip port]", argv[0]);
        return 0;
    }

    client::ChatClient client;
    client.Init(argv[1], atoi(argv[2]));
    std::cout << "输入用户名：";
    std::string name;
    std::string school;
    std::cin >> name;
    std::cout << "输入学校：";
    std::cin >> school;
    client.SetUserInfo(name, school);

    //创建两个线程，分别用来发送消息，和接收消息
    pthread_t stid, rtid;
    pthread_create(&stid, NULL, Send, &client);
    pthread_create(&rtid, NULL, Recv, &client);
    pthread_join(stid, NULL);
    pthread_join(rtid, NULL);

    return 0;
}
#endif
