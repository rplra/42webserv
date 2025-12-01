#include "Webserv.hpp"

int handleResponse(std::map<int, int>& clientServerMap, std::map<int, Request>& clientRequests, 
    std::map<int, std::string>& clientSendBuffers, std::map<int, const Server*>& socketServerMap,
    size_t& i, size_t& fds_count, std::vector<pollfd>& fds) {

    const Server*   clientServer = socketServerMap[clientServerMap[fds[i].fd]];
    Request&        request = clientRequests[fds[i].fd];
    std::string     req;
    char            buffer[1024];
    size_t          totalReceived = 0;
    ssize_t         bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0);

    if (bytes == 0) {
        // Connection closed by client
        std::cout << YELLOW << "Client disconnected (fd " << fds[i].fd << ")" << RESET << std::endl;
        closeClient(i, fds_count, fds, clientRequests, clientServerMap);
        return 1; // Skip to next fd
    } else if (bytes < 0) {
        std::cerr << RED << "recv() error on fd " << fds[i].fd << RESET << std::endl;
        closeClient(i, fds_count, fds, clientRequests, clientServerMap);
        return 1;
    } else {
        totalReceived += bytes;
        req.append(buffer, bytes);
        std::cout << GREEN << "Buffer: " << RESET << req << std::endl; // DEBUG
        request.readRequest(req);

        // Ensure the entire request is parsed before proceeding
        if (request.isParseComplete()) {
            std::cout << GREEN << "Location for path: " << request.getPath() << RESET << std::endl;
            const Location* locPath = clientServer->bestMatchingLocation(request.getPath());
            std::cout << GREEN << "Best matching location: " << RESET << (locPath ? locPath->path : "None") << std::endl;
            std::cout << GREEN << "index: " << RESET << (locPath ? locPath->index : "None") << std::endl;

            if (locPath) {
                std::string path = sendData(clientServer, request, locPath, totalReceived);
                clientSendBuffers[fds[i].fd] = path;
                fds[i].events |= POLLOUT; // enable write event
            }
        }                       
    }

    return 0;
}

std::string sendData(const Server* clientServer, const Request& client, const Location* locPath, size_t totalReceived) {
    std::string fullPath = clientServer->getRoot() + client.getPath();
    if (locPath && locPath->root != "")
        fullPath = locPath->root + client.getPath();

    std::cout << GREEN << "Full path before checks: " << RESET << fullPath << std::endl;
    std::cout << GREEN << "Directory ? " << RESET << clientServer->isDirectory(fullPath) << std::endl;

    // check client_max_body_size in location first, then server
    if (locPath && locPath->client_max_body_size > 0) {
        std::string result = checkMaxBodySize(clientServer, totalReceived, locPath->client_max_body_size, fullPath, locPath);
        if (result != "")
            return result;
    }
    else {
        std::string result = checkMaxBodySize(clientServer, totalReceived, clientServer->getClientMaxBodySize(), "", nullptr);
        if (result != "")
            return result;
    }

    if (clientServer->getIndex() != "" && client.getPath() == "/") {
        std::string fullPath = clientServer->getRoot() + "/" + clientServer->getIndex();
        return createResponse(fullPath, 200);
    }
    else if (clientServer->isDirectory(fullPath)) {
        if (locPath && locPath->index != "") {
            std::cout << GREEN << "Full path to resource: " << RESET << fullPath << std::endl;
            if (fullPath[fullPath.length() - 1] != '/')
                fullPath += "/";
            std::string indexPath = fullPath + locPath->index;
            return createResponse(indexPath, 200);
        }
        else if (locPath && locPath->autoindex) {
            return generateAutoindexPage(fullPath, client.getPath());
        }
        else {
            if (locPath && locPath->error_pages.find(403) != locPath->error_pages.end()) {
                std::string errorPagePath = fullPath + "/" + locPath->error_pages.at(403);             
                std::cout << GREEN << "error Page path: " << RESET << errorPagePath << std::endl;
                return createResponse(errorPagePath, 403);

            }
            std::string errorPagePath = clientServer->getRoot() + "/" + clientServer->getErrorPagePath(403);
            return createResponse(errorPagePath, 403);
        }
    }
    else if (locPath && locPath->redirect.size() > 0) {
        std::map<int, std::string>::const_iterator it = locPath->redirect.begin();
        int redirectCode = it->first;
        std::string redirectPath = it->second;
        std::cout << GREEN << "Redirecting to: " << RESET << redirectPath << " with code " << redirectCode << std::endl;
        return createRedirectResponse(redirectPath, redirectCode);
    }
    else if (locPath && clientServer->isFile(fullPath)) {
        std::cout << GREEN << "Full path to resource: " << RESET << fullPath << std::endl;
        return createResponse(fullPath, 200);
    }
    else {
        std::string errorPagePath = clientServer->getRoot() + "/" + clientServer->getErrorPagePath(404);
        return createResponse(errorPagePath, 404);
    }

    return "";
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << RED << "Error opening file: " << filePath << RESET << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // read the whole file into a buffer
    file.close();

    return buffer.str();
}

std::string createResponse(std::string filePath, int statusCode) {
    std::string body = readFile(filePath);
    if (body == "") {
        body = readFile("var/error.html"); // default error page
        statusCode = 500;
    }

    std::cout << GREEN << "Status Code: " << RESET << statusCode << std::endl;
    std::string statusText;
    if (statusCode == 200)
        statusText = "200 OK";
    else if (statusCode == 201)
        statusText = "201 Created";
    else if (statusCode == 301)
        statusText = "301 Moved Permanently";
    else if (statusCode == 404) 
        statusText = "404 Page Not Found";
    else if (statusCode == 403)
        statusText = "403 Forbidden";
    else if (statusCode == 500 || body == "") 
        statusText = "500 Internal Server Error";
    else 
        statusText = "Unknown Status";

    size_t pos = body.find("{{Status}}");
    if (pos != std::string::npos) {
        body.replace(pos, 10, statusText);
    }

    std::stringstream response;
    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;

    return response.str();
}

std::string generateAutoindexPage(const std::string& dirPath, const std::string& requestPath) {
    std::string body = "<html><head><title>Index of " + requestPath + "</title></head><body>";
    body += "<h1>Index of " + requestPath + "</h1><ul>";

    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        std::cerr << RED << "Error: Unable to open directory for autoindex" << RESET << std::endl;
        return createResponse("var/error.html", 500);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == ".") continue; // skip current directory
        body += "<li><a href=\"" + requestPath;
        if (requestPath.back() != '/')
            body += "/";
        body += name + "\">" + name + "</a></li>";
    }
    closedir(dir);

    body += "</ul></body></html>";

    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;

    return response.str();
}

std::string createRedirectResponse(const std::string& redirectPath, int statusCode) {
    std::string response;
    response += "HTTP/1.1 " + std::to_string(statusCode) + " Moved Permanently\r\n";
    response += "Location: " + redirectPath + "\r\n";
    response += "Content-Length: 0\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";

    return response;
}

void sendResponse(size_t& i, size_t& fds_count, std::vector<pollfd>& fds, 
    std::map<int, Request>& clientRequests, 
    std::map<int, int>& clientServerMap, 
    std::map<int, std::string>& clientSendBuffers)
{
    std::string& sendBuffer = clientSendBuffers[fds[i].fd];
    // std::cout << GREEN << "Send Buffer: " << RESET << sendBuffer << std::endl; // DEBUG
    ssize_t bytesSent = send(fds[i].fd, sendBuffer.c_str(), sendBuffer.size(), 0);
    if (bytesSent > 0) {
        sendBuffer.erase(0, bytesSent);
        if (sendBuffer.empty()) {
            fds[i].events &= ~POLLOUT; // remove write event
            closeClient(i, fds_count, fds, clientRequests, clientServerMap);
            std::cout << GREEN << "i: " << RESET << i << std::endl;
            std::cout << GREEN << "fds_count: " << RESET << fds_count << std::endl;
        }
    } else if (bytesSent < 0) {
        std::cerr << RED << "send() error on fd " << fds[i].fd << RESET << std::endl;
        closeClient(i, fds_count, fds, clientRequests, clientServerMap);
    } else {
        // bytesSent == 0, connection closed
        std::cerr << RED << "Connection closed while sending on fd " << fds[i].fd << RESET << std::endl;
        closeClient(i, fds_count, fds, clientRequests, clientServerMap);
    }
}