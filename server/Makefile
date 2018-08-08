chat_server: server.cc chat_server_main.cc
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp  #为了使用auto

.PHONY: clean

clean:
	rm chat_server
