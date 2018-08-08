# ChatSystem
基于UDP协议，实现多人聊天系统，用户通过下载客户端，就可以在服务器与多人进行群聊
服务器模块：
    Ĵ基于生产者消费者模型，创建消费者线程和生产者线程
    生产者线程从socket中读取数据并将其放在BlockQueue中
    消费者线程从BlockQueue中读取数据并且广播给每一位在线好友（online_friend_list_）
  BlockQueue:
    为交易场所，采用POSIX信号量来实现生产者与消费者访问交易场所的安全性。
   
