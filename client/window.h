#pragma once
#include <ncurses.h>
#include <locale.h>
#include <deque>
#include <iostream>
#include <unordered_set>

namespace client
{
class Window
{

public:
    //用来保存每个窗口的结构体指针
    WINDOW* header_win_;
    WINDOW* input_win_;
    WINDOW* output_win_;
    WINDOW* friend_list_win_;
    Window();
    ~Window();

    //绘制标题子窗口
    void DrawHeader();
    //绘制输入框
    void DrawInput();
    //绘制输出框
    void DrawOutput();
    //绘制好友列表
    void DrawFriendList();
    //往窗口中写字符串
    void PutStrToWin(WINDOW* win, int y, int x, const std::string& str);
    //从窗口中读字符串
    void GetStrFronWin(WINDOW* win, std::string* str);
    //
    void AddMsg(const std::string& msg);
    void AddFriend(const std::string& friend_info);
    void DelFriend(const std::string& friend_info);

private:

    //需要在输出窗口显示的消息
    std::deque<std::string> msgs_;
    //当前在线好友列表
    std::unordered_set<std::string> friend_list_;
};
}
