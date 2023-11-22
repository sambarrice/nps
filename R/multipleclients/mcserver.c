//mcserver
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080
#define MAX_CLIENTS 5

void handle_client(int client_socket) {
    char buffer[1024];
    int n;
    
    while (1) {
        n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            close(client_socket);
            return;
        }
        buffer[n] = '\0';
        printf("Received from client: %s", buffer);
        send(client_socket, buffer, n, 0);
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is listening on port %d\n", PORT);
    
    while (1) {
        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Acceptance failed");
            exit(EXIT_FAILURE);
        }
        
        // Fork a child process to handle the client
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        
        if (child_pid == 0) {
            // In the child process
            close(server_socket); // Close the server socket in the child
            handle_client(client_socket); // Handle the client request
            exit(0); // Exit the child process
        } else {
            // In the parent process
            close(client_socket); // Close the client socket in the parent
        }
    }
    
    close(server_socket);
    
    return 0;
}
