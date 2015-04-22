#include "Server.h"
#include <thread>

Server serva;
std::thread readthread, writethread;

void readfunc(){
	serva.UpdateRecv();
}

void writefunc(){
	serva.UpdateSend();
	exit(0);
}

int main(){
	serva.CreateServer();

	std::thread readthread = std::thread(readfunc);
	std::thread writethread = std::thread(writefunc);

	readthread.join();
	writethread.join();

	return 0;
}
