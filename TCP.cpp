#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h>    
#include <cstring>     
#include <fstream>
using namespace std;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Socket failed!\n";
        return 1;
    }

    struct sockaddr_in address; // builtin struct
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // any ip allowed
    address.sin_port = htons(8080);      // Port number 8080

    int check = ::bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    cout << "check value " << check << "\n";
    if (check < 0) {
        cerr << "Bind failed!" << "\n";
        return 1;
    }

    // 3 is backlog
    int ls = listen(server_fd, 3);
    cout << "listen value " << ls << "\n";
    if (ls < 0) {
        cerr << "Listen failed!" << "\n";
        return 1;
    }

    cout << "Server is listening on port 8080...\n";

    // Accept a connection
    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    cout << "new socket " << new_socket << "\n";
    if (new_socket < 0) {
        cerr << "Accept failed!\n";
        return 1;
    }

    // receive a message
    char buffer[1024] = {0};
    int value = recv(new_socket, buffer, 1024, 0); // default behavior, pop the data
    
    if (value > 0) {
        cout << "Message from client: " << buffer << "\n";

        ofstream output_file("message.txt", ios::app);
        if (!output_file) 
        {
            cerr << "Failed to open message.txt\n";
        } 
        else 
        {
            output_file.write(buffer, value);
            output_file << '\n';
            output_file.close();
            cout << "Done.";
        }
    }

    close(new_socket);  
    close(server_fd);   

    return 0;
}
