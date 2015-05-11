#include "Client.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include "cipher.h"

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_line(char *buffer, int bsize) {
    int ch, len;

    fgets(buffer, bsize, stdin);

    /* remove unwanted characters from the buffer */
    buffer[strcspn(buffer, "\r\n")] = '\0';

    len = strlen(buffer);

    /* clean input buffer if needed */
    if(len == bsize - 1)
        while ((ch = getchar()) != '\n' && ch != EOF);

    return len;
}

Client::Client(){

}

Client::~Client(){
}

void Client::InitClient(){
  char hostname[MAXDATASIZE];
	//Get hostname
	printf("Specify the hostname/IP you would like to connect to: ");
	if (get_line(hostname, MAXDATASIZE) == -1){
		perror("hostname");
		return;
	}
	ConnectToServer(hostname);
}

void Client::ConnectToServer(const char* _hostname){
	//Connect to hostname
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(_hostname, PORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("client: socket");
			continue;
		}
		fcntl(sockfd, F_SETFL, O_NONBLOCK);
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to bind socket\n");
		return;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); //all done with this structure

	//Get user's name
	printf("Welcome to the Chat server. What is your name? ");
	name = new char[MAXDATASIZE];
	int ress = get_line(name, MAXDATASIZE);
	if (ress == -1 || strlen(name) == 0){
		perror("getname");
		const char* anonName = "Anon";
		strncpy(name, anonName, MAXDATASIZE);
	}

	//let server know that we're here
	std::string joinMessage = std::string("0 ") + name + " joined.";
	if ((numbytes = sendto(sockfd, joinMessage.c_str(), joinMessage.length(), 0,
				p->ai_addr, p->ai_addrlen)) == -1) {
			perror("talker: sendto");
			exit(1);
	}

	clientRunning = true;

	printf("Welcome %s!\n", name);
}

void Client::SendUpdate(){
	char message[MAXDATASIZE];
	while (clientRunning){
		int messageLen = get_line(message, MAXDATASIZE);
		if (messageLen == -1) {
			return;
		}
		if (strcmp(message, "/logout") == 0){
			//let server know that we're gone like the wind
			std::string leaveMessage = std::string(name) + " left.";
			SendMessage('2', leaveMessage);
			printf("Logged out\n");
			clientRunning = false;
			CloseConnection();
			return;
		}else{
			SendMessage('1', std::string(message));
		}
	}
}

void Client::ReceiveUpdate(){
	while (clientRunning){
		numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1, 0, p->ai_addr, &p->ai_addrlen);
		if (numbytes != -1){
			ReceiveMessage(std::string(buf));
		}
	}
}

void Client::SendMessage(char _cmd, std::string _message){
	//encrypt then send to server
	std::string fullMessage = cipher::encrypt(std::string(1, _cmd) + ":" + name + ":" + _message, -3);
	if ((numbytes = sendto(sockfd, fullMessage.c_str(), fullMessage.length(), 0,
				p->ai_addr, p->ai_addrlen)) == -1) {
			perror("talker: sendto");
			exit(1);
	}
}

void Client::ReceiveMessage(std::string _incomingMessage){
	//Making sure there isn't a newline at the end
	_incomingMessage[numbytes] = '\0';
	//we will be splitting the incoming message by delimiter
	std::istringstream iss(_incomingMessage);
	std::vector<std::string> linesSplit;
	while (std::getline(iss, _incomingMessage, ':')){
		linesSplit.push_back(_incomingMessage);
	}
	//first string should be command
	//second should be name
	//third should be actual message
	if (linesSplit.size() >= 3){
		if (linesSplit[1] == "4"){
			//client exit
			clientRunning = false;
		}
	}
	if (linesSplit.size() >= 3){
		printf("\r(%s): %s\n", linesSplit[1].c_str(), linesSplit[2].c_str());
	}
}

void Client::CloseConnection(){
	close(sockfd);
}
