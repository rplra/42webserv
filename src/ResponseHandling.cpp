#include "Webserv.hpp"

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

std::string sendResponse(std::string filePath, int statusCode) {
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

void closeAllFd(std::vector<pollfd>& fds) {
    std::cout << RED << "Closing all file descriptors..." << RESET << std::endl;
    for (size_t i=0; i < fds.size(); i++) {
        close(fds[i].fd);
    }
}

