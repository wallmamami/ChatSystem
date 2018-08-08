#include "server.h"
#include <sstream>

namespace server
{
int ChatServer::Start(char* addr, short port)
{
    //启动服务器，并且进入事件循环
    //使用UDP作为通信协议
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_ < 0)
    {
        perror("socket");
        return -1;
    }

    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = inet_addr(addr);
    local.sin_port = htons(port);

    if((bind(sock_, (sockaddr*)&local, sizeof(local))) < 0)
    {
        perror("bind");
        return -1;
    }

    pthread_t productor, consumer;
    pthread_create(&productor, NULL, Product, this);
    pthread_create(&consumer, NULL, Consum, this);
    pthread_join(productor, NULL);
    pthread_join(consumer, NULL);
    
    close(sock_);
    return 0;
}

void* ChatServer::Product(void* arg)
{
    //生产者线程，主要做的事情读取sockeet中的数据，并且写入到BlockQueue
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    
    while(true)
    {
        //服务器线程，所以死循环一直从socket读取数据
        server->RecvMsg();
    }

    return NULL;
}

void* ChatServer::Consum(void* arg)
{
    
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    
    while(true)
    {
        server->BroadCast();
    }

    return NULL;
}

int ChatServer::RecvMsg()
{
    //1.从socket读取数据
    char buf[1024 * 5] = {0};
    sockaddr_in peer;
    socklen_t len = sizeof(peer);

    ssize_t s = recvfrom(sock_, buf, sizeof(buf)-1, 0, (sockaddr*)&peer, &len);
    if(s < 0)
    {
        perror("recvfrom");
        return -1;
    }
    buf[s] = '\0';
    
    Context context;
    context.str = buf;
    context.addr = peer;
    //2.把数据插入到BlockQueue
    queue_.PushBack(context);//string有char数组的构造函数
    return 0;
}

int ChatServer::BroadCast()
{
    //1.从队列中读取一个数据(为可无端经过序列化后的数据)
    Context context;
    queue_.PopFront(&context);

    //2.对读取的字符串数据进行反序列化
    Data data;
    data.UnSerialize(context.str);//将str中序列化的数据反序列化到data中
    
    //3.根据消息更新在线好友列表
    if(data.cmd == "quit")
    {
        // 2）如果这个成员是一个下线消息（command为特殊值）
        //    将其从好友列表中删除
        DelUser(context.addr, data.name);
    }
    else
    {
        // 1）如果这个成员当前不在好友列表里，将其添加进去
        AddUser(context.addr, data.name);
    }
    //4.遍历在线好友，把这个数据发送给每一个在线客户端(包括发送这条消息用户本身)
    //  (因为发送消息的用户也存在于好友队列，因此也会给他自己发送消息，从而达到发送者
    //  能够看到自己发送消息的效果，这种方式虽然达到目的，但是这里可以控制客户端，不经过
    //   网络传输就可以实现这个功能)
    
    for(auto item : online_friend_list_)
    {
        SendMsg(context.str, item.second);//直接发送已经序列化的数据
    }
    return 0;
}

void ChatServer::AddUser(sockaddr_in addr, const std::string& name)
{
    //这里的key相当于对ip地址进和用户名进行拼接
    //之所以使用name和ip地址共同进行拼接，本质上是因为仅仅使用ip可能会出现重复ip
    //的情况(如果N个客户端处于同一个局域网中，那么服务器端看到的ip地址就是相同的)
    std::stringstream ss;//(把一个string输出到string中,拼接)
    
    //wangjia:1231243142131231
    ss << name << ":" << addr.sin_addr.s_addr;
    std::string key;

    //[]对于map和unordered_map
    //1.如果不存在，就插入
    //2.如果存在就修改
    //
    //insert函数的返回值类型是一个键值对，第一个为迭代器，表示插入成功后的位置，第二个为bool 表示成功与失败
    online_friend_list_[ss.str()] = addr; //key为拼接好的str，值为addr

    return;
}

void ChatServer::DelUser(sockaddr_in addr, const std::string& name)
{
    std::stringstream ss;

    //wangjia:1231243142131231
    ss << name << ":" << addr.sin_addr.s_addr;
    online_friend_list_.erase(ss.str());

    return;
}


void ChatServer::SendMsg(const std::string& data, sockaddr_in addr)
{
    //把这个数据通过 sendto 发送给客户端
    sendto(sock_, data.data(), data.size(), 0, (sockaddr*)&addr, sizeof(addr));//data()也是string中的方法
}

}

