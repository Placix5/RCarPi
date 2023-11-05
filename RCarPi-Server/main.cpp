#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using std::cout;
using std::endl;

//Servidor

int main() {
	WSADATA wsaData;
	int iResult;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout<<("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cout << ("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	// Create a SOCKET for the server to listen for client connections

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		cout << ("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cout << ("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << ("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket;

	ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << ("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout<<("Bytes received: %d\n", iResult);
			cout << recvbuf;
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				cout << ("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			cout << ("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			cout << ("Connection closing...\n");
		else {
			cout << ("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	return 0;
}