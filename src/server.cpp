#include "Webserv.hpp"

/* 
    Flow of a server-side socket programming 
    1. socket() - create a socket
    2. bind() - bind the socket to an IP/port
    3. listen() - limits how many connections can wait before being accepted
    4. accept() - accept a connection
    5. recv() - receive data from a connection
    6. close() - close the connection

    1. set up server socket and get the port number from config file 
    2. receive request from client and store in a struct
    3. process the request and generate a response
*/
int main() {
    // Create socket 
    // AF_INET for IPv4, SOCK_STREAM for TCP
    // Server server; 
    Request request;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << RED << "Error creating socket" << RESET << std::endl;
        return 1;
    }

    // Attach the socket to the port 
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // replace with config port 
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the specified IP/port
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << RED << "Error binding socket" << RESET << std::endl;
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        std::cerr << RED << "Error listening on socket" << RESET << std::endl;
        close(serverSocket);
        return 1;
    }

    // Non-blocking mode for server socket
    // accept() uses serverSocket to wait for connection
    // If serverSocket is blocking, accept() will freeze the whole server
    // Retrieve current flag of the socket and set O_NONBLOCK 
    int flags = fcntl(serverSocket, F_GETFL, 0);
    fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

    // Wait for incoming connections and handle requests
    // In order to let the server run continuously, use poll() with infinite loop to monitor the server socket 
    // Reason of using std::vector is because we does not know how many clients will connect
    // Hence we can dynamically add client fds to the vector
    std::vector<pollfd> fds;

    pollfd server_fd;
    server_fd.fd = serverSocket;
    server_fd.events = POLLIN; // reading data
    fds.push_back(server_fd);

    // Infinite loop to keep the server running 
    while (true) {
        // Wait up to 5 sec for any fd in fds to be ready
        // select() uses fd_set which has limitation on max fds (1024) 
        // select() can also be used here but poll() is more scalable for a large number of fds as poll() does not have fd limitations
        int ready_count = poll(fds.data(), fds.size(), 5000);
        std::cout << "Poll returned: " << ready_count << std::endl; // DEBUG
        if (ready_count < 0) {
            std::cerr << RED << "Error in poll()" << RESET << std::endl;
            close(serverSocket);
            return 1;
        }

        std::cout << GREEN << "Server Event: " << RESET << fds[0].revents << std::endl; // DEBUG
        for (size_t i = 0; i < fds.size(); ++i) {
            // keep listening for new incoming connections
            if ((i == 0) & (fds[i].revents & POLLIN)) {
                std::cout << GREEN << "Incoming connection detected" << RESET << std::endl;
                if (handleRequest(serverSocket, fds) != 0) {
                    std::cerr << RED << "Error in handling request" << RESET << std::endl;
                    return 1;
                }
            }
            // keep reading data from connected clients
            else if ((i != 0) & (fds[i].revents & POLLIN)) {
                // Receive and parse request from the client
                std::string req;
                size_t maxBodySize = 1024 * 1024 ; // replace with config value
                size_t totalReceived = 0;
                char buffer[1024];
                ssize_t bytes;

                while ((bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0)) > 0) {
                    totalReceived += bytes;
                    std::cout << "Bytes received: " << bytes << std::endl;
                    // Limit client body size based on config file 
                    if (totalReceived >= maxBodySize) {
                        std::cerr << RED << "Error: Request body too large" << RESET << std::endl;
                        close(fds[i].fd);
                        return 1;
                    }
                    req.append(buffer, bytes);
                    std::cout << GREEN << "Buffer: " << RESET << req << std::endl;

                    // need to terminate when the end of the HTTP request is reached
                    request.readRequest(req);
                    std::cout << "\nMethod : " << request.getMethod() << std::endl;
                    std::cout << "Path   : " << request.getPath() <<std::endl;
                    
                    std::cout << "\nHeaders: " << std::endl;
                    for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
                        std::cout << it->first << ":" << it->second << std::endl;
                    
                    std::cout << "\nCookies: " << std::endl;
                    for (std::map<std::string, std::string>::const_iterator it = request.getCookies().begin(); it != request.getCookies().end(); ++it)
                        std::cout << it->first << ":" << it->second << std::endl;

                    std::cout << "\nParse Complete? " << (request.isParseComplete() ? "Yes" : "No") << std::endl;
                    std::cout << "----------------------------------------" << std::endl;
                }

                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                // adjust index after erasing element as the remaining elements shift left 
                i--;
            }
        }
    }
    
    close(serverSocket);
}

int handleRequest(int serverSocket, std::vector<pollfd>& fds) {
    // Accept a connection
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    
    pollfd client_fd;
    client_fd.fd = clientSocket;
    client_fd.events = POLLIN; // reading data    
    fds.push_back(client_fd);
    
    // Non-blocking mode for client socket
    // recv() uses clientSocket to read data
    // If clientSocket is blocking, recv() will freeze the whole server
    // Retrieve current flags of the socket and set O_NONBLOCK
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);
    
    std::cout << GREEN << "Connection accepted" << RESET << std::endl;

    // compare host header and server_name (handle multiple server blocks)
    // Now only one server block is handled so we use default directly
    // generate response
    // std::string path = req.getPath(); // replace with request path
    // Location* best_match = getBestMatchingLocation(path, server);
    // if (!best_match) {
    //     std::string errorMsg = getErrorPagePath(404, server);
    //     send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
    // }

    // std::cout << GREEN << "Best matching location: " << RESET << best_match->path << std::endl;
    // std::string fullPath = "";
    // if (isDirectory(path)) {
    //     fullPath = best_match->root + best_match->index;
    // }
    // else if (isFile(path)) {
    //     fullPath = best_match->root + path;
    // }
    // std::cout << GREEN << "Full path to resource: " << RESET << fullPath << std::endl;

    // Close the both socket
    return 0;
}

// Location* getBestMatchingLocation(const std::string& requestPath, const Server& server) {
//     Location* best_match = nullptr;
//     size_t best_length = 0;
//     for (size_t i = 0; i < server.locations.size(); ++i) {
//         const Location& loc = server.locations[i];
//         size_t len = loc.path.length();
//         if (loc.path.find(requestPath) == 0) {
//             // match with the longest prefix
//             if (len > best_length) {
//                 best_length = len;
//                 best_match = &server.locations[i];
//             }
//         }
//     }

//     return best_match;
// }

// bool isDirectory(const std::string& path) {
//     struct stat pathStat;
//     if (stat(path.c_str(), &pathStat) != 0) {
//         return false; // error accessing path
//     }

//     return S_ISDIR(pathStat.st_mode);
// }

// bool isFile(const std::string& path) {
//     struct stat pathStat;
//     if (stat(path.c_str(), &pathStat) != 0) {
//         return false; // error accessing path
//     }

//     // Check if it's a regular file
//     return S_ISREG(pathStat.st_mode);
// }

// std::string getErrorPagePath(int errorCode, const Server& server) {
//     std::map<int, std::string>::const_iterator it = server.error_pages.find(errorCode);
//     std::map<int, std::string>::const_iterator default_it = server.error_pages.find(500);

//     if (it != server.error_pages.end()) {
//         return it->second;
//     }

//     return default_it->second;
// }