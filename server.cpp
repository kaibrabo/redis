#include <cstddef>
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/errno.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
void die(std::string s, int fd)
{
	std::cerr << s << " " << "failed; Errno: " << errno << std::endl;
	close(fd);
}

void doSomething(int fd)
{
	std::cout << "Do Something: " << fd << std::endl;
	char readBuffer[64] = {}; // list of chars, not string
	ssize_t readBufferLength = read(fd, (void *)readBuffer, sizeof(readBuffer) - 1);

	if (readBufferLength < 0)
	{
		die("read()", fd);
	}
}

void createTcpServer()
{
	// create socket
	int socketFd = socket(PF_INET, SOCK_STREAM, 0);
	int val = 1;

	// set socket options
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	// bind name to socket
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = htonl(0);

	if (bind(socketFd, (const struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		die("bind()", socketFd);
	}

	// listen to connection
	int backlog = 4096;
	if (listen(socketFd, backlog) < 0)
	{
		die("listen()", socketFd);
	}

	// accept connection
	while (1) {
		struct sockaddr_in clientAddr = {};
		socklen_t addrLength = sizeof(clientAddr);
		int connectionFd = accept(socketFd, (struct sockaddr *)&clientAddr, &addrLength);

		if (connectionFd < 0)
		{
			continue; // error
		}

		doSomething(connectionFd);

		close(connectionFd);
	}
}