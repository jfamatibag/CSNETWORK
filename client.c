#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

struct Packet {
    char name[100];
    int number;
};

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    
    struct Packet clientPacket;
    struct Packet serverPacket;
    int inputNumber;
    int bytesReceived;
    int finalSum;
    int exit_status = 0;

    printf("Enter an integer between 1 and 100 (or out-of-range to shutdown server): ");
    if (scanf("%d", &inputNumber) != 1) {
        printf("[ERROR] Failed to parse integer input.\n");
        exit_status = 1;
    }
    printf("[INPUT] User entered integer value: %d\n", inputNumber);

    strcpy(clientPacket.name, "Client of Name Test");
    clientPacket.number = inputNumber;

    printf("[INIT] Initializing Winsock library components...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[ERROR] Winsock initialization failed.\n");
        exit_status = 1;
    }
    printf("[SUCCESS] Winsock library active.\n");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("[ERROR] Socket creation failed.\n");
        WSACleanup();
        exit_status = 1;
    }
    printf("[SUCCESS] Client socket created.\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("[ERROR] Invalid address format.\n");
        closesocket(sock);
        WSACleanup();
        exit_status = 1;
    }

    printf("[ACTION] Attempting active TCP handshake connection to 127.0.0.1:%d...\n", PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("[ERROR] Connection Failed.\n");
        closesocket(sock);
        WSACleanup();
        exit_status = 1;
    }
    printf("[SUCCESS] Connected to server successfully.\n");

    printf("[ACTION] Transmitting packet data payload to server...\n");
    send(sock, (char*)&clientPacket, sizeof(clientPacket), 0);
    printf("[SUCCESS] Transmitted payload containing client name and number successfully.\n");

    if (inputNumber < 1 || inputNumber > 100) {
        printf("[STATUS] Value out of range. Server has been notified to terminate.\n");
        printf("[CLEANUP] Releasing client socket handles and exiting cleanly.\n");
        closesocket(sock);
        WSACleanup();
        exit_status = 0;
    }

    printf("[WAITING] Blocking execution until server reply packet is received...\n");
    bytesReceived = recv(sock, (char*)&serverPacket, sizeof(serverPacket), 0);
    
    if (bytesReceived > 0) {
        printf("[RECEIVED] Successfully received %d bytes from the server socket.\n", bytesReceived);
        
        finalSum = clientPacket.number + serverPacket.number;
        printf("\n");
        printf("Client Name:             %s\n", clientPacket.name);
        printf("Server Name (Extracted): %s\n", serverPacket.name);
        printf("Client's Integer Value:  %d\n", clientPacket.number);
        printf("Server's Integer Value:  %d\n", serverPacket.number);
        printf("Calculated Local Sum:    %d\n", finalSum);
        printf("\n");
    } else {
        printf("[ERROR] Did not get a clean response data frame from the server.\n");
    }

    printf("[CLEANUP] Closing local client socket handles...\n");
    closesocket(sock);
    printf("[CLEANUP] WSACleanup() to de-allocate network libraries...\n");
    WSACleanup();
    printf("[STATUS] Execution complete. Process terminating.\n");

    return exit_status;
}