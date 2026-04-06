#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sstream>

#define PORT 8080

int main() {
    
    // SERVER
    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT) // host to netwoek
    };

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Setting socket option to reuse port/address immediately after restart
    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)); 

    if (server_fd == -1) {
        std::cerr << "Not able to create your socket! Getting out\n";
        return -1;
    }

    // Bind socket
    bind(server_fd, (struct sockaddr*)&saddr, sizeof(saddr)); // assigns the address and port here to netwoek

    // Listening
    std::cout << "THE VALUE OF SOMAXCONN --> " << SOMAXCONN << "\n";
    listen(server_fd, SOMAXCONN);

    std::stringstream ss;
    ss << PORT;
    std::cout << "Listening on port " << ss.str() << "\n";

    // CLIENT
    struct sockaddr_in caddr;
    socklen_t caddrSize = sizeof(caddr);
    int socketClient;

    char buff[4096];
    int received_bytes;

    // waiting for client
    while (true) {
        // Accept connections from clients
        socketClient = accept(server_fd, (struct sockaddr*)&caddr, &caddrSize);
        std::cout << "Client connected with success.\n";

        // Try to find out who is the client
        char hostClient[NI_MAXHOST];
        char portClient[NI_MAXSERV];
        memset(hostClient, 0, NI_MAXHOST);
        memset(portClient, 0, NI_MAXSERV);

        if (getnameinfo((struct sockaddr*)&caddr, sizeof(caddr), hostClient, NI_MAXHOST, portClient, NI_MAXSERV, 0) == 0) {
            std::cout << "reverse dns hit\n";
            std::cout << " --> " << hostClient << " connected to the port " << portClient << std::endl;
        } 
        else 
        {
            inet_ntop(AF_INET, &caddr.sin_addr, hostClient, NI_MAXHOST);
            std::cout << "raw ip hit\n";
            std::cout << " --> " << hostClient << " connected to the port " << ntohs(caddr.sin_port) << std::endl;
        }

        // Receive our data
        memset(buff, 0, 4096);
        received_bytes = recv(socketClient, buff, 4096, 0);

        if (received_bytes == -1) 
        {
            std::cerr << "Error receiving message. Quitting" << std::endl;
            break;
        } 
        else if (received_bytes == 0) 
        {
            std::cout << "Client Disconnected" << std::endl;
            break;
        }

        // using netcat to send data
        send(socketClient, buff, received_bytes + 1, 0);

        // Print the message
        std::cout << "Message: " << std::string(buff, 0, received_bytes) << std::endl;

        close(socketClient);
    }

    close(server_fd);
    return 0;
}