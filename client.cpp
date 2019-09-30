/*	AG0907 Lab 3 TCP client example - by Henry Fortuna and Adam Sampson

	A simple client that connects to a server and waits for
	a response. The server sends "hello" when the client first
	connects. Text typed is then sent to the server which echos
	it back, and the response is printed out.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")


// The IP address of the server to connect to
#define SERVERIP "127.0.0.1"

// The TCP port number on the server to connect to
#define SERVERPORT 5555

// The (fixed) size of message that we send between the two programs
#define MIN_MESSAGE_SIZE 1

#define TERM_CHAR '*' //askii contains a character to designate end of transmission that no one can type in their message

// Prototypes
void die(const char *message);


int main()
{
	printf("Client Program\n");

	// Initialise the WinSock library -- we want version 2.2.
	WSADATA w;
	int error = WSAStartup(0x0202, &w);
	if (error != 0)
	{
		die("WSAStartup failed");
	}
	if (w.wVersion != 0x0202)
	{
		die("Wrong WinSock version");
	}

	// Create a TCP socket that we'll connect to the server
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		die("socket failed");
	}

	// Fill out a sockaddr_in structure with the address that
	// we want to connect to.
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	// htons converts the port number to network byte order (big-endian).
	addr.sin_port = htons(SERVERPORT);
	addr.sin_addr.s_addr = inet_addr(SERVERIP);

	// inet_ntoa formats an IP address as a string.
	printf("IP address to connect to: %s\n", inet_ntoa(addr.sin_addr));
	// ntohs does the opposite of htons.
	printf("Port number to connect to: %d\n\n", ntohs(addr.sin_port));

	// Connect the socket to the server.
	if (connect(sock, (const sockaddr *)&addr, sizeof addr) == SOCKET_ERROR)
	{
		die("connect failed");
	}

	printf("Connected to server\n");

	// Make buffer a pointer so size can be determined
	char * buffer;

	while (true)
	{
		printf("Type some text (\"quit\" to exit): ");
		fflush(stdout);

		// Read a line of text from the user.
		std::string line;
		std::getline(std::cin, line);

		//Append termination character to end of string
		line.push_back(TERM_CHAR);

		buffer = new char[line.size()]; //Make buffer a dinamic array big enough to hold the text entered

		
		memcpy(buffer, line.c_str(), line.size());

		// Send the message to the server.
		if (send(sock, buffer, line.size(), 0) != line.size())
		{
			die("send failed");
		}

		delete buffer;

		
		// Read a response back from the server.
		buffer = new char[MIN_MESSAGE_SIZE]; //Repurpuse buffer as a 1 character long char array so it can be used to receve characters one at a time
		std::string response; //Holds the totall response receved

		while (true) {
			int count = recv(sock, buffer, MIN_MESSAGE_SIZE, 0);
			std::cout << buffer[0] << std::endl;
			if (count <= 0)
			{
				printf("Server closed connection\n");
				break;
			}
			if (buffer[0] == TERM_CHAR) {
				printf("Reached termination character\n");
				break;
			}
			
			response.push_back(buffer[0]);
		}

		delete buffer;

		printf("Received %d bytes from the server: '", response.size());
		std::cout << response;

		printf("'\n");
	}

	printf("Quitting\n");

	// Close the socket and clean up the sockets library.
	closesocket(sock);
	WSACleanup();

	return 0;
}


// Print an error message and exit.
void die(const char *message)
{
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}