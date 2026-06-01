#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // links to the Windows socket library

#define PORT 8080
#define SERVER_NUM 67 

struct Packet {
    char name[50];
    int number;
};

int main() {
    WSADATA wsaData;
    SOCKET server_fd = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in address;
    struct Packet receivedPacket;   
    struct Packet replyPacket;      
    int addrlen = sizeof(address);
    int server_running = 1;
    int bytesRead;
    int exit_status = 0;
    char *serverName = "Server of John Q. Smith";

    printf("[INITIALIZATION] ...\n");

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[ERROR] WSAStartup failed.\n");
        exit_status = 1;
    }
    printf("[SUCCESS] WSAStartup initialized successfully.\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("[ERROR] Socket creation failed.\n");
        WSACleanup();
        exit_status = 1;
    }
    printf("[SUCCESS] Server socket created.\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("[ERROR] Bind failed.\n");
        closesocket(server_fd);
        WSACleanup();
        exit_status = 1;
    }
    printf("[SUCCESS] Socket successfully bound to Port %d.\n", PORT);

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("[ERROR] Listen failed.\n");
        closesocket(server_fd);
        WSACleanup();
        exit_status = 1;
    }
    printf("[STATUS] %s is now actively listening for incoming connections...\n", serverName);

    while (server_running) {
        
        printf("\n[WAITING] Awaiting a new client connection request...\n");
        client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        
        if (client_socket != INVALID_SOCKET) {
            printf("[CONNECTION] Client connected successfully!\n");

            printf("[ACTION] Attempting to read payload structure...\n");
            bytesRead = recv(client_socket, (char*)&receivedPacket, sizeof(receivedPacket), 0);
            
            if (bytesRead > 0) {
                printf("[RECEIVED] Successfully read %d bytes from the client.\n", bytesRead);

                if (receivedPacket.number < 1 || receivedPacket.number > 100) {
                    printf("[TERMINATION] Out-of-range integer (%d).\n", receivedPacket.number);
                    server_running = 0; 
                } else {
                    printf("\n");
                    printf("Client Name (Extracted): %s\n", receivedPacket.name);
                    printf("Server Name:             %s\n", serverName);
                    printf("Client's Number:         %d\n", receivedPacket.number);
                    printf("Server's Picked Number:  %d\n", SERVER_NUM);
                    printf("Sum:        %d\n", receivedPacket.number + SERVER_NUM);
                    printf("\n");

                    strcpy(replyPacket.name, serverName);
                    replyPacket.number = SERVER_NUM;

                    printf("[ACTION] Sending reply payload back to client...\n");
                    send(client_socket, (char*)&replyPacket, sizeof(replyPacket), 0);
                    printf("[SUCCESS] Reply successfully sent to client.\n");
                }
            } else {
                printf("[ERROR] Failed to receive data or client disconnected.\n");
            }

            closesocket(client_socket);
            printf("[CLEANUP] Client connection handle closed.\n");
        } else {
            printf("[ERROR] Accept failed.\n");
        }
    }

    printf("\n[TERMINATION] Closing server socket...\n");
    closesocket(server_fd);
    printf("[TERMINATION] WSACleanup() to de-allocate network libraries...\n");
    WSACleanup();
    printf("[TERMINATION] Server shutdown complete. Exiting...\n");

    return exit_status;
}
