#include "window.h"
#include <string>

namespace client
{
Window::Window()
{
    //设置字符编码“中文”
    setlocale(LC_ALL, "");
    //对整个ncurses 进行初始化
    initscr();
    //隐藏光标
    curs_set(0);
}

void Window::DrawHeader()
{
    //LINES 时ncurses 提供的宏，表示当前窗口的最大行数
    int h = LINES/5;
    
    //COLS也是ncurses 提供的宏，表示当前窗口的最大列数
    int w = COLS;

    //x y 表示窗口起始位置
    int y = 0;
    int x = 0;
    header_win_ = newwin(h, w, y, x);

    //给窗口加边框
    box(header_win_, '|', '-');

    std::string title = "网络聊天室";
    //往窗口中写字符串
    PutStrToWin(header_win_, h/2, w/2-title.size()/2, title);
    wrefresh(header_win_);
}

void Window::PutStrToWin(WINDOW* win, int y, int x, const std::string& str)
{
    mvwaddstr(win, y, x, str.c_str());
}
    //从窗口中读字符串
void Window::GetStrFronWin(WINDOW* win, std::string* str)
{
    //可以用string 的resize来优化，减少拷贝次数
    char buf[1024 * 10] = {0};
    wgetnstr(win, buf, sizeof(buf)-1);
    *str = buf;
}


//绘制输入框
void Window::DrawInput()
{
    //LINES 时ncurses 提供的宏，表示当前窗口的最大行数
    int h = LINES/5;
    
    //COLS也是ncurses 提供的宏，表示当前窗口的最大列数
    int w = COLS;

    //x y 表示窗口起始位置
    int y = LINES * 4 / 5;
    int x = 0;
    input_win_ = newwin(h, w, y, x);

    std::string title = "请输入消息：";
    //此时的坐标其实是窗口左上角位原点的坐标系
    //次数我们从（1，1）开始填充提示符，是为了不和边框重复
    //因为边框已经占用了第0行和第0列
    PutStrToWin(input_win_, 1, 1, title);
    //给窗口加边框
    box(input_win_, '|', '-');

    wrefresh(input_win_);
    
}


//绘制输出框
void Window::DrawOutput()
{
    int h = LINES * 3 / 5;
    int w = COLS * 3 / 4;
    int y = LINES /5;
    int x = 0;

    output_win_ = newwin(h, w, y, x);
    box(output_win_, '|', '-');

    for(size_t i = 0; i < msgs_.size(); ++i)
    {
        PutStrToWin(output_win_, i + 1, 2, msgs_[i]);
    }
    wrefresh(output_win_);
}


void Window::AddMsg(const std::string& msg)
{
    //此处由于我们的窗口显示的消息条数有限，不能无止境的插入新消息
    //当msgs包含的消息数目超过一定的阈值，就把旧消息删除掉。
    msgs_.push_back(msg);
    if(msgs_.size() > 20)//最多存20条
    {
        msgs_.pop_front();
    }
}


//绘制好友列表
void Window::DrawFriendList()
{
    int h = LINES * 3 / 5;
    int w = COLS / 4;
    int y = LINES / 5;
    int x = COLS * 3 / 4;
    friend_list_win_ = newwin(h, w, y, x);
    box(friend_list_win_, '|', '-');
    //TODO 遍历好友列表, 把好友信息显示出来
    size_t i = 0;
    for(auto item : friend_list_)
    {
        PutStrToWin(friend_list_win_, i + 1, 1, item);    
        ++i;
    }

    wrefresh(friend_list_win_);
}

void Window::AddFriend(const std::string& friend_info)
{
    friend_list_.insert(friend_info);
}

void Window::DelFriend(const std::string& friend_info)
{
    friend_list_.erase(friend_info);
}

Window::~Window()
{
    //d对整个ncurses 进行销毁动作
    endwin(); //一定要销毁，如果忘记，终端会显示混乱
}

}//end client

#ifdef TEST_WINDOW
#include <unistd.h>
int main()
{
    client::Window win;
    win.DrawHeader();
    win.DrawInput();
    win.AddMsg("wangjia: hehe");
    win.AddMsg("wangjia: haha");
    win.AddMsg("wangjia: hehe");
    win.AddMsg("wangjia: hehe");
    win.AddMsg("wangjia: haha");
    win.AddMsg("wangjia: haha");
    win.DrawOutput();
    win.AddFriend("wangjia|shankeda");
    win.AddFriend("wangjia|shankeda");
    win.AddFriend("wangjia|shankeda");
    win.AddFriend("wangjia|shankeda");
    win.DrawFriendList();

    sleep(5);

    return 0;
}

#endif
