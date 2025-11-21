## webserv
A fully functional HTTP/1.1 web server implemented in **C++98**, inspired by the behavior of **NGINX**, and compliant with the requirements of the 42 curriculum.

This project covers low-level networking using **sockets**, **non-blocking I/O**, **poll()**, HTTP request/response parsing, CGI execution, configuration handling, and serving static/dynamic content.


#### Technical Specs

| Component            | Specification          |
|----------------------|------------------------|
| Protocol 	           | HTTP/1.1               |
| Client (Browser)     | Google Chrome          |
| Reference Server     | NGINX                  |
| Language             | C++ 98                 |
| I/O Multiplexing     | `poll()`               |
| Configuration        | NGINX-style            |
| CGI Scripts          | tbc (python/php/etc)   |
| Build System         | `Makefile`             |


#### Features
- Fully non-blocking server using **poll()**
- Multi-port & multi-server configuration
- HTTP/1.1 compliant request handling
- Methods supported: **GET, POST, DELETE**  
- Chunked transfer decoding
- Keep-alive connections
- Redirects & error pages
- CGI execution
- Static file serving
- Directory listing (autoindex)
- Session handling & cookies
- Config file parsing (NGINX-like syntax)


#### Documentation
[Notion : Webserv](https://www.notion.so/webserv-2a76434f644c80399b33c64b8c4dac1f) 