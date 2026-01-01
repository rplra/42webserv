#include "Webserv.hpp"
#include "ConfigParse.hpp"

void	Response::setEnvVariables()
{
	// get absolute path for PHP-CGI script
	std::ostringstream oss;
	std::string currentDir; 
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
			currentDir = cwd;
	} else {
		perror("getcwd() error");
	}

	_env_variables.push_back("REQUEST_METHOD=" + _request->getMethod());
	// _env_variables.push_back("CONTENT_LENGTH=" + std::to_string(_request->getBody().length()));
	oss <<_request->getBody().length();
	_env_variables.push_back("CONTENT_LENGTH=" + oss.str());
	oss.str("");
	oss.clear();
	if (_request->getMethod() == "POST")
		_env_variables.push_back("CONTENT_TYPE=" + _request->getHeaders().at("content-type"));
	if (_request->getMethod() == "GET")
		_env_variables.push_back("QUERY_STRING=" + _request->getQuery());
	_env_variables.push_back("SCRIPT_NAME=" + _request->getPath());
	_env_variables.push_back("SERVER_NAME=" + _server.getHost());
	// _env_variables.push_back("SERVER_PORT=" + std::to_string(_server.getPort()));
	oss << _server.getPort();
	_env_variables.push_back("SERVER_PORT=" + oss.str());
	oss.str("");
	oss.clear();
	_env_variables.push_back("SCRIPT_FILENAME=" + currentDir + "/" + _file_path); // for PHP
	_env_variables.push_back("REDIRECT_STATUS=200"); // for PHP
	_env_variables.push_back("SERVER_PROTOCOL=HTTP/1.1");
	if (_request->hasSessionId())
		_env_variables.push_back("HTTP_COOKIE=" + _request->getHeaders().at("cookie")); // for cookie
	else
		_env_variables.push_back("HTTP_COOKIE="); // empty cookie

	/*debug*/std::cout << PURPLE << "CGI Environment Variables: " << RESET << std::endl;
	for (std::vector<std::string>::const_iterator it = _env_variables.begin(); it != _env_variables.end(); ++it) {
		std::cout << *it << std::endl;
	}
}

void	Response::executeCgi(const Location* location, int len)
{
	int stdin_pipe[2]; // parent writes to child (body)
	int stdout_pipe[2]; // child writes to parent (cgi response)

	pipe(stdin_pipe);
	pipe(stdout_pipe);
	if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
		std::cerr << RED << "Pipe Error" << RESET << std::endl;
        return;
	}

	pid_t pid = fork();
	if(pid == 0) 
	{
		// CGI program know nothing about socket, HTTP connection and server 
		// create two pipes so that stdin -> input (req) to cgi program, stdout -> output (res) from cgi program
		dup2(stdin_pipe[0], STDIN_FILENO);
		dup2(stdout_pipe[1], STDOUT_FILENO);

		std::vector<char*> cgi_args;
		for (std::vector<std::string>::const_iterator it = _env_variables.begin(); it != _env_variables.end(); ++it) {
			cgi_args.push_back(const_cast<char*>(it->c_str()));
		}
		cgi_args.push_back(NULL);
		
		if (_request->getPath().compare(len - 3, 3, ".py") == 0) {
			_cgi_path = location->_cgi.at(PY);          
		}
		else if (_request->getPath().compare(len - 4, 4, ".php") == 0) {
            _cgi_path = location->_cgi.at(PHP);
		}
        else {
            perror("Unsupported CGI script type");
            exit(1);
        }
        
        std::vector<char*> argv_arr;
        argv_arr.push_back(const_cast<char*>(_cgi_path.c_str()));
        argv_arr.push_back(const_cast<char*>(_file_path.c_str()));
        argv_arr.push_back(NULL);

        if (execve(_cgi_path.c_str(), argv_arr.data(), cgi_args.data()) == -1) {
            perror("Failed to execute CGI script");
            exit(1);
        } 
	}

	close(stdin_pipe[0]);
	close(stdout_pipe[1]);

	write(stdin_pipe[1], _request->getBody().c_str(), _request->getBody().length());

	ssize_t nbytes;
	char buffer[4096];
	std::string cgiResponse;

    // Setup non-blocking read
    int flag = fcntl(stdout_pipe[0], F_GETFL, 0);
    fcntl(stdout_pipe[0], F_SETFL, flag | O_NONBLOCK);

    // Setup pollfd
    struct pollfd pfd;
    pfd.fd = stdout_pipe[0];
    pfd.events = POLLIN;

    // timeout for poll
    const int timeout = 2000; // 2 seconds
    int poll_result = poll(&pfd, 1, timeout);

    if (poll_result > 0) {
		while (true)
		{
			nbytes = read(stdout_pipe[0], buffer, sizeof(buffer));
			if (nbytes > 0) {
				cgiResponse.append(buffer, nbytes);
			}
			else if (nbytes == 0)
			{
				// further process if is php
				if (_request->getPath().compare(len - 4, 4, ".php") == 0) {
					size_t start = cgiResponse.find("<html>");
					// /*debug*/std::cout  <<"cgiR: " << cgiResponse << std::endl;
					_cgiResponse = cgiResponse.substr(start);
				}
				else
					_cgiResponse = cgiResponse;
				break ;
			}
		}
    }
	else if (poll_result == 0) {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);  // clean up zombie
        std::cerr << RED << "CGI script timed out" << RESET << std::endl;
    } else {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);  // clean up zombie
        std::cerr << RED << ERR_POLL << RESET << std::endl;
    }

	std::cout << GREEN << "CGI Response: \n" << RESET << _cgiResponse << std::endl;

	close(stdin_pipe[1]);
	close(stdout_pipe[0]);

	int status;
	waitpid(pid, &status, 0);
}