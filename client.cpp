#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>


#define BUFFER_SIZE 256
#define SERVER_PORT 8000
#define LOCALHOST "127.0.0.1"


int start_transfer(int& sock, FILE* file)
{
	int len, sentcount;
	char buffer[BUFFER_SIZE + 1];
	while(len = fread(buffer, sizeof(char), BUFFER_SIZE, file))
	{
		if ((sentcount = send(sock, buffer, len, 0)) == -1)
		{
			perror("Send error");
		}
	}
	if (ferror(file))
	{
		perror("Reading file error");
		return 0;
	}
	if (feof(file))
	{
		perror("EOF found");
	}
	return 1;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: [filename]" << std::endl;
		exit(1);
	}
	
	FILE* file = fopen(argv[1], "r");
	if (file == NULL)
	{
		perror("Bad filename");
		exit(1);
	}
	
	int csock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERVER_PORT);
	saddr.sin_addr.s_addr = inet_addr(LOCALHOST);

	if (connect(csock, (sockaddr*)&saddr, sizeof(saddr)) == -1)
	{
		perror("Connect error");
		exit(1);
	}
	else std::cout << "Connected to server" << std::endl;

	if (!start_transfer(csock, file))
	{
		std::cout << "Error in transfer func" << std::endl;
	}

	shutdown(csock, SHUT_RDWR);
	close(csock);

	return 0;
}

