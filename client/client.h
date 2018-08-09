#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../server/api.hpp"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

namespace client
{

class ChatClient
{

public:
    //客户端初始化, 主要是进行socket的初始化
    int Init(const std::string& ip, short port);

    //设置用户信息, 姓名和学校
    //客户端启动时提示用户输入姓名学校
    int SetUserInfo(const std::string& name, const std::string& school);

    //发送消息(用户输入一条消息，发送一条消息, 但是要将其拼接为特定格式，还要将其用Json序列化)
    //客户端和服务器交互的接口要严格按照服务器提供的api来操作
    //这样的约定就相当于自定制了一种应用层协议
    void SendMsg(const std::string& msg);

    //接收消息--接收的消息是经过Json序列化后的数据
    void RecvMsg(server::Data* data);
private:

    //用来保存和服务器交互的socket，通过Init函数创建好，交给SendMsg和RecvMsg直接使用
    int sock_;
    sockaddr_in server_addr_; //保存服务器的信息

    std::string name_;
    std::string school_;
};   

} //end client
