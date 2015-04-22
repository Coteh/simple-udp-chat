#ifndef _CHAT_CLIENT
#define _CHAT_CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXDATASIZE 100
#define PORT "6666" //port we are connecting to

class Client {
	public:
		Client();
		~Client();

    void InitClient();
    void ConnectToServer(const char* _hostname);
		void SendUpdate();
		void ReceiveUpdate();
    void SendMessage(char _cmd, std::string _message);
    void ReceiveMessage(std::string _incomingMessage);
		void CloseConnection();

		bool clientRunning;
	private:
		char* name;
		int sockfd;
		struct addrinfo hints, *servinfo, *p;
		struct sockaddr_storage remoteaddr; //server address
		socklen_t remoteaddrlen;

		int numbytes;
		char buf[MAXDATASIZE];
		int rv;
};

#endif //_CHAT_CLIENT
