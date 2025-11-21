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
        Server server; 
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

        while (true) {
            if (handleRequest(serverSocket, server) != 0) {
                std::cerr << RED << "Error accepting connection" << RESET << std::endl;
            }
        }
        
        close(serverSocket);
}

int handleRequest(int serverSocket, Server& server) {
    // Accept a connection
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0) {
        std::string errorMsg = getErrorPagePath(500, server);
        send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
        close(serverSocket);
        return 1;
    }

    // Receive and parse request from the client
    std::string request;
    size_t maxBodySize = 6660; // replace with config value
    size_t totalReceived = 0;
    char buffer[1024];
    size_t bytes;

    while ((bytes = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        totalReceived += bytes;
        std::cout << "Bytes received: " << bytes << std::endl;
        // Limit client body size based on config file 
        if (totalReceived >= maxBodySize) {
            std::cerr << RED << "Error: Request body too large" << RESET << std::endl;
            close(clientSocket);
            break;
        }
        request.append(buffer, bytes);
        std::cout << GREEN << "Buffer: " << RESET << request << std::endl;
        // need to terminate when the end of the HTTP request is reached
    }
    std::cout << GREEN << "Received data: " << RESET << request << std::endl;

    // compare host header and server_name (handle multiple server blocks)
    // Now only one server block is handled so we use default directly
    // generate response
    std::string path = request.getPath(); // replace with request path
    Location* best_match = getBestMatchingLocation(path, server);
    if (!best_match) {
        std::string errorMsg = getErrorPagePath(404, server);
        send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
    }

    std::cout << GREEN << "Best matching location: " << RESET << best_match->path << std::endl;
    std::string fullPath = "";
    if (isDirectory(path)) {
        fullPath = best_match->root + best_match->index;
    }
    else if (isFile(path)) {
        fullPath = best_match->root + path;
    }
    std::cout << GREEN << "Full path to resource: " << RESET << fullPath << std::endl;

    // Close the both socket
    close(clientSocket);
    return 0;
}

Location* getBestMatchingLocation(const std::string& requestPath, const Server& server) {
    Location* best_match = nullptr;
    size_t best_length = 0;
    for (size_t i = 0; i < server.locations.size(); ++i) {
        const Location& loc = server.locations[i];
        size_t len = loc.path.length();
        if (loc.path.find(requestPath) == 0) {
            // match with the longest prefix
            if (len > best_length) {
                best_length = len;
                best_match = &server.locations[i];
            }
        }
    }

    return best_match;
}

bool isDirectory(const std::string& path) {
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0) {
        return false; // error accessing path
    }

    return S_ISDIR(pathStat.st_mode);
}

bool isFile(const std::string& path) {
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0) {
        return false; // error accessing path
    }

    // Check if it's a regular file
    return S_ISREG(pathStat.st_mode);
}

std::string getErrorPagePath(int errorCode, const Server& server) {
    std::map<int, std::string>::const_iterator it = server.error_pages.find(errorCode);
    std::map<int, std::string>::const_iterator default_it = server.error_pages.find(500);

    if (it != server.error_pages.end()) {
        return it->second;
    }

    return default_it->second;
}