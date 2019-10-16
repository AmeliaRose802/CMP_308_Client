/*	AG0907 Lab 4 UDP client example - by Henry Fortuna and Adam Sampson

	When the user types a message, the client sends it to the server
	as a UDP packet. The server then sends a packet back to the
	client, and the client prints it out.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <winsock2.h>
#include "Message.h"
#include <Windows.h>
#include <time.h>
#include <map>

#pragma comment(lib, "ws2_32.lib")


// The IP address of the server
#define SERVERIP "127.0.0.1"

// The UDP port number on the server
#define SERVERPORT 4444

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40
#define LOOP_DELAY 1000

// Prototypes
void die(const char *message);

void printMap(std::map<int, GameObject> objects) {
	for (auto elem : objects)
	{
		elem.second.print();
	}
}


int main()
{
	//Create the readible set
	fd_set readable;
	FD_ZERO(&readable);

	timeval timeout;

	std::map<int, GameObject> gameObjects;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;


	srand(time(NULL));
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

	// Create a UDP socket.
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		die("socket failed");
	}

	// Fill out a sockaddr_in structure with the address that
	// we want to send to.
	sockaddr_in toAddr;
	toAddr.sin_family = AF_INET;
	// htons converts the port number to network byte order (big-endian).
	toAddr.sin_port = htons(SERVERPORT);
	toAddr.sin_addr.s_addr = inet_addr(SERVERIP);

	// inet_ntoa formats an IP address as a string.
	printf("IP address to send to: %s\n", inet_ntoa(toAddr.sin_addr));
	// ntohs does the opposite of htons.
	printf("Port number to send to: %d\n\n", ntohs(toAddr.sin_port));

	// We'll use this buffer to hold the messages we exchange with the server.
	char buffer[MESSAGESIZE];

	bool shouldLoop = true;

	int t = 0;

	//Create an object in the game and assign it a random ID
	GameObject player(rand(), 0, 0);

	std::cout << "MY OBJECT: ";
	player.print();

	//This is going to send messages as fast as it can hear back from server or at least every 1 secound. Not sure if more delay is desirable
	while (shouldLoop) {
		FD_SET(sock, &readable);

		player.moveCircle(t);

		Message msg(player.id, player.x, player.y);

		if (sendto(sock, (const char *)&msg, sizeof(Message), 0, (const sockaddr *)&toAddr, sizeof(toAddr)) != sizeof(Message))
		{
			die("sendto failed");
		}


		int count = select(0, &readable, NULL, NULL, &timeout);
		if (count < 0) {
			die("Select failed");
		}

		//This works so long as the server starts before the client. Otherwise it fails. 
		if (FD_ISSET(sock, &readable))
		{
			count = recvfrom(sock, (char *)&msg, sizeof(Message), 0, NULL, NULL);

			
			
			if (count < 0)
			{
				die("recvfrom failed");
			}
			if (count != sizeof(Message))
			{
				die("received odd-sized message");
			}


			//Check if the gameobject list has heard of this object before
			if (gameObjects.count(msg.objectID)) {
				gameObjects[msg.objectID].update(msg.x, msg.y);
			}
			else {
				std::cout << "New object\n";
				gameObjects.insert(std::pair<int, GameObject>(msg.objectID, GameObject(msg.objectID, msg.x, msg.y)));
			}



			//Keeps it only sending every 1 sec even if it heard back sooner
			Sleep(LOOP_DELAY);
		}

		printMap(gameObjects);

		t += LOOP_DELAY;
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