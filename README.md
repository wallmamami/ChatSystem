# ChatSystem
版本一：

基于UDP协议，实现多人聊天系统，用户通过下载客户端，就可以在服务器与多人进行群聊
服务器模块：
    Ĵ基于生产者消费者模型，创建消费者线程和生产者线程
    生产者线程从socket中读取数据并将其放在BlockQueue中
    消费者线程从BlockQueue中读取数据并且广播给每一位在线好友（online_friend_list_）
  BlockQueue:
    为交易场所，采用POSIX信号量来实现生产者与消费者访问交易场所的安全性。

服务器与客户端交互接口
    Data{
            name
            school
            msg
            cmd
        }
    使用jsoncpp进行序列化和反序列化(优点：可读性好，方便调试、解析 缺点：浪费空间，需要额外花费空间去组织)
    序列化：value对象中的Writer方法
    反序列化：value对象中的Reader方法
    
客户端模块:
    创建两个线程
    一个线程用来从socket读取消息，读取的消息为经过Json序列化的，还要通过Json反序列化
    另一个线程用来发送消息，发送的消息也要经过Json序列化(相当于自定制了应用层协议)
