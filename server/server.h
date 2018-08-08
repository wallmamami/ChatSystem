#pragma once
#include <string>
#include <unordered_map>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "block_queue.hpp"
#include "api.hpp"
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;


namespace server
{

//Blockqueue中的数据类型
struct Context
{
    std::string str;
    sockaddr_in addr;
};
    
class ChatServer
{
public:
    int Start(char* addr, short port);//用来启动服务器

    //接收消息
    int RecvMsg();

    //广播消息
    int BroadCast();

private:
    static void* Consum(void*); //消费者线程
    static void* Product(void*); //生产者线程

    void AddUser(sockaddr_in addr, const std::string& name);
    void DelUser(sockaddr_in addr, const std::string& name);

    void SendMsg(const std::string& data, sockaddr_in addr);

    //key 内容为用户的身份标识，ip+昵称，如果只是ip，有可能多个用户ip相同
    //value ip+端口号(struct sockaddr_in用来给用户发消息)
    std::unordered_map<std::string, sockaddr_in> online_friend_list_; //在线好友列表
    //实现一个队列，用来保存消息，作为交易场所(使用阻塞队列) 即若队列为空，消费者阻塞等待，队列已满，生产者线程阻塞等待
    //暂定队列中元素类型为std::string, 生产者线程从socket读取数据放到队列中，消费者线程从队列读拿数据并广播。
    BlockQueue<Context> queue_;
    int sock_; 
};

}//end server
