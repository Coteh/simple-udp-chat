#include "Server.h"
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

Server::Server(){
	sockfd = 0;
	isServerRunning = true;
}

Server::~Server(){

}

void Server::CreateServer(){
	//get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; //use my IP

	if ((returnedValue = getaddrinfo(NULL, PORT, &hints, &addressInfo)) != 0){
		fprintf(stderr, "selectserver: %s\n", gai_strerror(returnedValue));
		exit(1);
	}

	for (point = addressInfo; point != NULL; point = point->ai_next) {
		sockfd = socket(point->ai_family, point->ai_socktype, point->ai_protocol);
		if (sockfd == -1){
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, point->ai_addr, point->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	//if we got here, it means we didn't get bound
	if (point == NULL){
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}
	freeaddrinfo(addressInfo); //all done with this
	remoteaddrlen = sizeof(remoteaddr);

	printf("We are NOW running!\n");
}

void Server::UpdateRecv(){
	while (isServerRunning){
		if ((nbytes = recvfrom(sockfd, buf, MAXDATASIZE-1 , 0,
				(struct sockaddr *)&remoteaddr, &remoteaddrlen)) == -1) {
					return;
		}
		if (nbytes > 0){
			HandleMessage();
		}
	}
}

void Server::UpdateSend(){
	char message[MAXDATASIZE];
	while (isServerRunning){
		int messageLen = get_line(message, MAXDATASIZE);
		if (messageLen == -1) {
			return;
		}else if (strcmp(message, "/end") == 0){
			//Send end message to all users
			BroadcastMessageToAll(std::string("4:Serva:Server has been shut down!"));
			//Shut the server down
			isServerRunning = false;
			return;
		}else{
			BroadcastMessageToAll(std::string("1:Serva:") + message);
		}
	}
}

void Server::HandleMessage(){
	//handle data from a client
	//decrypt buf
	std::string decryptedStr = cipher::decrypt(std::string(buf), -3);
	char code = decryptedStr[0];
	if (code == '0'){ //client is logging on
		printf("Client joined.\n");
		//Add client's address to the vector
		clientsVec.push_back(*(struct sockaddr*)&remoteaddr);
		printf("clients size: %i\n", int(clientsVec.size()));
	}else if (code == '1'){ //client is sending a message
		//printf("listener: packet is %d bytes long\n", nbytes);
		decryptedStr[nbytes] = '\0';
		printf("(%s): %s\n", inet_ntop(remoteaddr.ss_family,
				get_in_addr((struct sockaddr *)&remoteaddr),
				remoteIP, sizeof remoteIP), decryptedStr.c_str());
		//send message to all other clients
		BroadcastMessageToAll(decryptedStr);
	}else if (code == '2'){ //client is logging out
		printf("Client left.\n");
		for (int i = 0; i < clientsVec.size(); i++){
			if(CompareClients(&clientsVec[i], (struct sockaddr *)&remoteaddr)){
				printf("Going to erase\n");
				clientsVec.erase(clientsVec.begin() + i);
				break;
			}
		}
		printf("clients size: %i\n", int(clientsVec.size()));
		//shut down server if everyone leaves
		if (clientsVec.size() == 0){
			return;
		}
	}
}

void Server::BroadcastMessage(std::string _message, struct sockaddr* _destination){
	remoteaddrlen = sizeof(*_destination);
	sendto(sockfd, _message.c_str(), _message.length(), 0, _destination, remoteaddrlen);
}

void Server::BroadcastMessageToAll(std::string _message){
	for (int i = 0; i < clientsVec.size(); i++){
		//disable the if check if you want to test on localhost
		// if(!CompareClients(&clientsVec[i], (struct sockaddr *)&remoteaddr)){
			BroadcastMessage(_message, &clientsVec[i]);
		// }
	}
}

bool Server::CompareClients(struct sockaddr* c_1, struct sockaddr* c_2){
	return (inet_ntop(remoteaddr.ss_family, get_in_addr(c_1), remoteIP, sizeof remoteIP) ==
	inet_ntop(remoteaddr.ss_family, get_in_addr(c_2), remoteIP, sizeof remoteIP));
}

void Server::CloseServer(){
	close(sockfd);
}
