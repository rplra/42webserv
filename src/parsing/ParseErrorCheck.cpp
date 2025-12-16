#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

bool	ConfigParser::isDirective(const std::string &word)
{
	const char *type[] =
	{
		"listen",
		"server_name",
		"root",
		"index",
		"autoindex",
		"error_page",
		"client_max_body_size",
		"location",
		"allowed_methods",
		"cgi_handler",
		"upload_store",
		"return",
		NULL
	};
	for (size_t i=0;  type[i];  i++)
	{
		if (type[i] == word)
			return (1);
	}
	return (0);
}


bool	ConfigParser::checkBraces(const std::string &to_find, std::vector<std::string> &data)
{
	std::vector<std::string>::iterator it = std::find(data.begin(), data.end(), to_find);
	if (it == data.end()) // if brace_not_found
	{
		if (to_find == "{")
			throw (std::invalid_argument(ERR_OPENBRACEMISSING));
		else if (to_find == "}")
			throw (std::invalid_argument(ERR_CLOSEBRACEMISSING));
	}
	return (1);
}

/* checks if line has only 1 brace, else throw error */
bool	ConfigParser::noMoreBrace(std::istringstream &iss)
{
	std::string word;
	int			count = 0;
	while (iss >> word)
		count++;
	if (count != 0)
	{
		this->_check.keyword = word;
		throw (std::invalid_argument(ERR_UNEXPECTSIGN));
	}
	return (1);
}

bool ConfigParser::ignoreKeyword(std::string &word, std::istringstream &iss, errCheckGroup &data)
{
	const char *arr[] =
	{
		"#",
		"{",
		NULL
	};

	for (size_t i=0;  arr[i];  i++)
	{
		if (word == arr[i])
		{
			// /*debug*/ std::cout << "ignoreKeyword: " << word << std::endl;
			if (i != 0 && noMoreBrace(iss))
				this->checkDuplicate(word, data.brace, ERR_UNEXPECTSIGN);
			return (1);
		}
	}
	return (0);
}

/* checks for duplicate directives in config file */
bool ConfigParser::checkDuplicate(std::string &str, std::vector<std::string> &data, const std::string err_message)
{
	std::vector<std::string>::iterator it = data.begin();
	std::vector<std::string>::iterator ite = data.end();

	while (it != ite)
	{
		if (*it == str)
		{
			this->_check.keyword = str;
			throw (std::invalid_argument(err_message));
		}
		it++;
	}
	data.push_back(str);
	return (1);
}

bool	ConfigParser::checkDuplicateCgi(std::string &str, std::istringstream &iss, std::map<std::string, std::string> &data)
{
	std::map<std::string, std::string>::iterator it = data.begin();
	std::map<std::string, std::string>::iterator ite = data.end();

	std::string			extension, path;
	std::istringstream	tmp_iss(iss.str());
	tmp_iss.seekg(iss.tellg());

	tmp_iss >> extension;
	tmp_iss >> path;

	while (it != ite)
	{
		if (it->first == extension && it->second == path)
		{
			this->_check.keyword = str;
			/*debug*/std::cout << "checkDuplicateCgi" << std::endl;
			throw (std::invalid_argument(ERR_DUPLICATE));
		}
		it++;
	}
	data[extension] = path;
	return (1);
}

/* checks for semicolon here */
int	ConfigParser::countArgs(std::istringstream &iss)
{
	std::string			word;
	int					count = 0;
	std::istringstream	tmp(iss.str());
	tmp.seekg(iss.tellg());

	while (tmp >> word)
	{
		count++;
		// if (word.find(';') != std::string::npos) // if found
			// return (count);
	}
	return (count);
	// throw (std::invalid_argument(ERR_SEMICOLONMISSING));
}

void	ConfigParser::checkCommonArgCount(size_t code, std::istringstream &iss)
{
	std::string word;
	int			count = countArgs(iss);

	// /*debug*/std::cout << ", common_arg_count: " << count << std::endl;
	switch (code)
	{
		case ERROR_PAGE:
		if (count == 2)
			return ;
		break ;

		case ROOT:
		case INDEX:
		case AUTOINDEX_DIR:
		case CLIENT_MAX_BODY_SIZE:
		if (count == 1)
			return ;
	}
	throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
}

void	ConfigParser::checkServerArgCount(size_t code, std::istringstream &iss)
{
	std::string word;
	int			count = 0;

	if (code != LOCATION)
		count = countArgs(iss);

	// /*debug*/std::cout << ", serv_arg_count: " << count << std::endl;
	switch (code)
	{
		case LISTEN:
		if (count == 1)
			return ;
		break;

		case SERVER_NAME:
		if (count >= 1)
			return ;
		break;
	}
	throw std::invalid_argument(ERR_ARGCOUNTINVALID);
}

/* return (0) == no error */
bool	ConfigParser::errorServerDirective(std::string &str, std::istringstream &iss, std::ifstream &inFile)
{
	(void) inFile;

	// if (str == "}")
		// return (0);

	const char *arr[] =
	{
		"listen",
		"server_name",
		"location"
	};
	std::vector<std::string> types(arr, arr + 3);
	for (std::size_t i=0; i < types.size(); i++)
	{
		if (types[i] == str)
		{
			if (i == LOCATION)
				errorCheckLocation(iss, inFile);
			else //errorCheckServer
			{
				// /*debug*/std::cout << "errorServerDirective" << std::endl;
				checkDuplicate(str, this->_check.serv.dup, ERR_DUPLICATE);
				checkServerArgCount(i, iss);
			}
			return (0);
		}
	}
	/*debug*/ std::cout << PINK << "errorServerDirective invalid: " << str << RESET << std::endl;
	return (1);
}

/* return (0) == no error */
bool	ConfigParser::errorCommonDirective(std::string &str, std::istringstream &iss, errCheckGroup &data)
{
	(void) iss;

	const char *arr[] =
	{
		"root",
		"index",
		"autoindex",
		"error_page",
		"client_max_body_size"
	};
	std::vector<std::string>	types(arr, arr + 5);

	for (std::size_t i=0; i < types.size(); i++)
	{
		if (types[i] == str)
		{
			// /*debug*/std::cout << "errorCommonDirective: " << str << std::endl;
			if (i != 3)
				checkDuplicate(str, data.dup, ERR_DUPLICATE);
			checkCommonArgCount(i, iss);
			checkValidTypeCommon(i, iss, data);
			return (0);
		}
	}
	// /*debug*/ std::cout << PINK << "errorCommonDirective invalid: " << str << RESET << std::endl;
	return (1); //type_not_found
}

bool	ConfigParser::checkTrimSemicolon(std::string &buffer)
{
	std::string			tmp;
	std::istringstream	iss(buffer);

	/* check keyword */
	iss >> tmp;
	if (tmp.empty() || tmp == "location" || tmp == "server" \
		|| tmp == "#" || tmp == "{" || tmp == "}")
	{
		return (0);
	}
	buffer = trimStringTail(buffer, '#');
	tmp = trimStringTail(buffer, ';');
	if (tmp == buffer) // if no semicolon
	{
		std::istringstream iss(buffer);
		iss >> this->_check.keyword;
		throw (std::invalid_argument(ERR_SEMICOLONMISSING));
	}
	// /*debug*/std::cout << PINK << "Semicolon: " << tmp << std::endl;
	return (1);
}

/* checks the server scope */
void	ConfigParser::errorCheckServer(std::istringstream &iss, std::ifstream &inFile)
{
	std::string			word, buffer;

	this->_check.serv.dup.clear();
	this->_check.serv.cgi.clear();
	this->_check.serv.brace.clear();
	this->_check.serv.b_openBrace = 0;
	this->_check.serv.b_closeBrace = 0;

	/* checks server base arg_count & {} */
	errorServerBase(iss);

	while (std::getline(inFile, buffer))
	{
		this->_check.line_count++;

		if (checkTrimSemicolon(buffer))
			buffer = trimStringTail(buffer, ';');

		iss.clear();
		iss.str(buffer);
		// /*debug*/ std::cout << YELLOW << buffer << RESET << std::endl;

		if (!(iss >> word) || ignoreKeyword(word, iss, this->_check.serv))
			continue ;
		this->_check.keyword = word;

		/* updates openBrace */
		if (this->_check.serv.b_openBrace == 0)
			this->_check.serv.b_openBrace = checkBraces("{", this->_check.serv.brace);

		if (word == "}" && noMoreBrace(iss))
			break ;

		if (errorCommonDirective(word, iss, this->_check.serv) && errorServerDirective(word, iss, inFile))
			throw (std::invalid_argument(ERR_DIRECTIVEINVALID)); //invalid_directive
	}
}

/* return 0 == no error */
bool	ConfigParser::errorServerBase(std::istringstream &iss)
{
	std::string word;
	int			count = 0;

	while (iss >> word)
	{
		this->_check.keyword = word;
		if (word == "{" || word == "}")
		{
			count++;
			checkDuplicate(word, this->_check.serv.brace, ERR_UNEXPECTSIGN);
		}
		else
			throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
	}
	if (count <= 1)
		return (0);
	throw (std::invalid_argument(ERR_UNEXPECTSIGN));
}

bool	ConfigParser::errorCheckConfig(std::ifstream &inFile)
{
	std::streampos		ori_pos = inFile.tellg();
	std::string			word, buffer;
	std::istringstream	iss;

	/* errorCheckServer */
	this->_check.line_count = 0;
	while (std::getline(inFile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		this->_check.line_count++;

		if (!(iss >> word) || word == "#")
			continue ;
		this->_check.keyword = word;
		if (word == "server")
			this->errorCheckServer(iss, inFile);
		else
		{
			if (isDirective(word))
				throw (std::invalid_argument(ERR_DIRECTIVENOTALLOW));
			throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
		}
	}
	/* restore original position after error checks */
	inFile.clear();
	inFile.seekg(ori_pos);
	return (0);
}
