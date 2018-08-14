.PHNOY: all

all: test_win test_client chat_client

chat_client: client.cc window.cc chat_client_main.cc
	g++ -o $@ $^ -lpthread -ljsoncpp -lncursesw -std=c++11

test_win: window.cc
	g++ -o $@ $^ -lpthread -lncursesw -std=c++11 -DTEST_WINDOW #定义宏
test_client: client.cc
	g++ -o $@ $^ -lpthread -ljsoncpp -DTEST_CHAT_CLIENT

.PHNOY: clean

clean:
	rm test_client test_win chat_client
