#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define RECEIVE_BUF 256
#define FILENAME "result.txt"

int receive_text(int& sock, FILE* file)
{
	int reccount;
	char buffer[RECEIVE_BUF + 1];
	while (reccount = recv(sock, buffer, RECEIVE_BUF, 0))
	{
		fwrite(buffer, sizeof(char), reccount, file);
	}
	if (reccount == -1)
	{
		perror("Recv text error");
		return 0;
	}
	return 1;
}

void daemon()
{
	int slisten = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in listenAddr;
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(SERVER_PORT);
	listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(slisten, (sockaddr*)&listenAddr, sizeof(listenAddr)) == -1)
	{
		perror("Bind error");
		return;
	}

	int optval = 1;
	setsockopt(slisten, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (listen(slisten, SOMAXCONN) == -1)
	{
		perror("Listen error");
		return;
	}
	
	sockaddr_in clientSockAddr;
	socklen_t addrlen = (socklen_t)sizeof(clientSockAddr);
	int serversock;	
	FILE* file;

	while(true)
	{
		file = fopen(FILENAME, "a");
		serversock = accept(slisten, (sockaddr*)&clientSockAddr, &addrlen);

		if (serversock == -1)
		{
			perror("Accept error");
		}
	
		if(!receive_text(serversock, file))
		{
			exit(1);
		}

		fclose(file);
	}

	shutdown(serversock, 2);
	close(slisten);
	close(serversock);
}

int main()
{
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("Fork error");
		exit(1);
	}
	else if (pid == 0)
	{
		daemon();
	}
	return 0;
}
