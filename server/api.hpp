#pragma once
#include <string>
#include <jsoncpp/json/json.h>


namespace server
{
//客户端给服务器提供的接口(api)
//如果服务器按照这种格式组织数据，服务器就可以正常接受并且
//解析以及转发
struct Data
{
    std::string name;
    std::string school;
    std::string msg;
    std::string cmd;

    //序列化函数
    void Serialize(std::string* output)
    {
        //可以自己手动序列化，但最好还是用成熟的第三方库(jsoncpp)
        // char buf[1024] = {0};
        // sprintf(buf, "{name:%s, school:%s, msg:%s, cmd:%s}", name.c_str(), school.c_str(), msg.c_str(), cmd.c_str());//%s匹配的是c风格的字符串
        // *output;
        Json::Value value;//(把value对象当作unordered_map来使用，用来保存数据)
        value["name"] = name;
        value["school"] = school;
        value["msg"] = msg;
        value["cmd"] = cmd;

        Json::FastWriter writer;//专门用来序列化的类
        *output = writer.write(value);
        return;
    }
    //反序列化函数
    void UnSerialize(const std::string& input)
    {
        Json::Value value;  //将反序列好的数据保存在value中
        Json::Reader reader;//用来反序列化的对象
        reader.parse(input, value);

        //因为如果value中的数据不是string，直接asString可能会有问题
        //所以可以先用valule["name"].isString判断
        name = value["name"].asString();//直接取出来不是string
        school = value["school"].asString();
        msg = value["msg"].asString();
        cmd = value["cmd"].asString();

        return;
    }
};

}
