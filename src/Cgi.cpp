#include "Webserv.hpp"
#include "ConfigParse.hpp"

bool 	Response::parseCgiHeaders(const std::string& raw, size_t &pos)
{
	size_t	line_end = 0;
	size_t	sep_len = 4;
	bool	flag = 0;
	
	size_t	headers_end = raw.find("\r\n\r\n");
	if (headers_end == std::string::npos) // if not found
	{
		headers_end = raw.find("\n\n");
		sep_len = 2;
	}
	else if (headers_end == std::string::npos)
		return (0); // no headers found

	while (pos < headers_end)
	{
		if (sep_len == 4)
			line_end = raw.find("\r\n", pos);
		else
			line_end = raw.find("\n", pos);

		if (line_end == std::string::npos || line_end > headers_end)
			break ;

		size_t colon = raw.find(':', pos);
		if (colon == std::string::npos || colon > line_end)
			break ;
		
		std::string key = trim(std::string(&raw[pos], colon - pos));
		key = toLower(key);
		std::string value = trim(std::string(&raw[colon + 1], line_end - (colon + 1)));
		// /* debug */std::cout << "> KEY:VALUE -> " << key << " : " << value << std::endl;

		if (!flag)
			flag = 1;
		if (key == "status")
		{
			size_t code = std::strtod(value.c_str(), NULL);
			setStatus(static_cast<HttpStatus>(code));
		}
		else if (key == "content-type")
			_content_type = value;

		pos = line_end + 1;
	}
	// /*debug*/std::cout << "leftbody: " << &raw[pos] << std::endl;
	// pos = headers_end + sep_len; // move cursor to body_start, skipping header_end empty line
	return (flag);
}

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
	if (_request->getMethod() == "GET" || _request->getMethod() == "DELETE")
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

	clock_t start = clock(); // start timer
    const double TIMEOUT = 2.0; // 2 seconds

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
				_cgiResponse = cgiResponse.substr(start);
			}
			else
				_cgiResponse = cgiResponse;
			break ;
		}

		// check elapsed time
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        if (elapsed > TIMEOUT)
        {
            std::cerr << RED << "CGI script timed out" << RESET << std::endl;
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            break;
        }
	}

	std::cout << GREEN << "CGI Response: \n" << RESET << _cgiResponse << std::endl;

	close(stdin_pipe[1]);
	close(stdout_pipe[0]);

	int status;
	waitpid(pid, &status, 0);
}
