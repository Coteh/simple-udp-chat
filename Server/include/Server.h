#ifndef _CHAT_SERVER
#define _CHAT_SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <string>

#define MAXDATASIZE 100
#define PORT "6666" //port we are listening to

class Server {
public:
	Server();
	~Server();

	void CreateServer();
	void RunServer();
	void UpdateRecv();
	void UpdateSend();
	void HandleMessage();
	void BroadcastMessage(std::string _message, struct sockaddr* _destination);
	void BroadcastMessageToAll(std::string _message);
	void CloseServer();

	bool isServerRunning;
private:
	int sockfd;
	struct sockaddr_storage remoteaddr; //client address
	socklen_t remoteaddrlen;
	char buf[MAXDATASIZE]; //buffer for client data
	int nbytes;
	char remoteIP[INET6_ADDRSTRLEN];

	int client, otherClients, returnedValue;

	struct addrinfo hints, *addressInfo, *point;

	std::vector<struct sockaddr> clientsVec;

	bool CompareClients(struct sockaddr* c_1, struct sockaddr* c_2);
};

#endif //_CHAT_SERVER
