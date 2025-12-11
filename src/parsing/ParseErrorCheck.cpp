#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

/* returns 1 if no repeat brace */
// bool	Config::addCheckBrace(std::string &word, std::vector<std::string> &data)
// {
// 	std::vector<std::string>::iterator it = std::find(data.begin(), data.end(), word);
// 	if (it != data.end()) // if scope exists
// 	{
// 		this->_check.keyword = word;
// 		throw (std::invalid_argument(ERR_UNEXPECTSIGN));
// 	}
// 	data.push_back(word);
// 	return (1);
// }

bool	Config::checkBraces(const std::string &to_find, std::vector<std::string> &data)
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
bool	Config::noMoreBrace(std::istringstream &iss)
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

bool Config::ignoreKeyword(std::string &word, std::istringstream &iss, errCheckGroup &data)
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
bool Config::checkDuplicate(std::string &str, std::vector<std::string> &data, const std::string err_message)
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

int	Config::countArgs(std::istringstream &iss)
{
	std::string			word;
	int					count = 0;
	std::istringstream	tmp(iss.str());
	tmp.seekg(iss.tellg());

	while (tmp >> word)
	{
		count++;
		if (word.find(';') != std::string::npos) // if found
			return (count);
	}
	throw (std::invalid_argument(ERR_SEMICOLONMISSING));
}

void	Config::checkCommonArgCount(size_t code, std::istringstream &iss)
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
		case AUTOINDEX:
		case CLIENT_MAX_BODY_SIZE:
		if (count == 1)
			return ;
	}
	throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
}

void	Config::checkServerArgCount(size_t code, std::istringstream &iss)
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
bool	Config::errorServerDirective(std::string &str, std::istringstream &iss, std::ifstream &inFile)
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
			try
			{
				if (i == LOCATION)
					errorCheckLocation(iss, inFile);
				else //errorCheckServer
				{
					checkDuplicate(str, this->_check.serv.dup, ERR_DUPLICATE);
					checkServerArgCount(i, iss);
				}
			}
			catch (std::exception &err)
			{
				/*debug*/ std::cout << PINK << "throw in errorServerDirective: " << str << RESET << std::endl;
				throw ;
			}
			return (0);
		}
	}
	/*debug*/ std::cout << PINK << "errorServerDirective invalid: " << str << RESET << std::endl;
	return (1);
}

/* return (0) == no error */
bool	Config::errorCommonDirective(std::string &str, std::istringstream &iss, std::vector<std::string> &data)
{
	// if (str == "}")
		// return (0);

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
			// /*debug*/ std::cout << str << std::endl;
			try
			{
				checkDuplicate(str, data, ERR_DUPLICATE);
				checkCommonArgCount(i, iss);
			}
			catch (std::exception &err)
			{
				/*debug*/ std::cout << RED << "errorCommonDirective" << RESET << std::endl;
				throw ;
			}
			/* check semicolon : end and no other chars */
			return (0);
		}
	}
	// /*debug*/ std::cout << PINK << "errorCommonDirective invalid: " << str << RESET << std::endl;
	return (1); //type_not_found
}

/* checks the server scope */
void	Config::errorCheckServer(std::istringstream &iss, std::ifstream &inFile)
{
	std::string			word, buffer;

	this->_check.serv.dup.clear();
	this->_check.serv.brace.clear();
	this->_check.serv.b_openBrace = 0;
	this->_check.serv.b_closeBrace = 0;

	/* checks server base arg_count & {} */
	errorServerBase(iss);

	while (std::getline(inFile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		this->_check.line_count++;
		// /*debug*/ std::cout << YELLOW << buffer << RESET << std::endl;

		if (!(iss >> word) || ignoreKeyword(word, iss, this->_check.serv))
			continue ;
		this->_check.keyword = word;

		/* updates openBrace */
		if (this->_check.serv.b_openBrace == 0)
			this->_check.serv.b_openBrace = checkBraces("{", this->_check.serv.brace);

		if (word == "}" && noMoreBrace(iss)) //(throw here)
			break ;

		try
		{
			if (errorCommonDirective(word, iss, this->_check.serv.dup) && errorServerDirective(word, iss, inFile))
				throw (std::invalid_argument(ERR_DIRECTIVEINVALID)); //invalid_directive
		}
		catch (std::exception &err)
		{
			std::cout << "errorCheckServer" << std::endl;
			throw ;
		}
	}
}

/* return 0 == no error */
bool	Config::errorServerBase(std::istringstream &iss)
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

bool	Config::errorCheckConfig(std::ifstream &inFile)
{
	std::streampos		ori_pos = inFile.tellg();
	std::string			word, buffer;
	std::istringstream	iss;

	/* errorCheckServer */
	while (std::getline(inFile, buffer))
	{
		this->_check.line_count++;
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		// /*debug*/ std::cout << "errorCheckConfig_word: " << word << std::endl; //throw invalid directive

		try
		{
			this->_check.keyword = word;
			if (word == "#")
				continue ;
			if (word == "server")
				this->errorCheckServer(iss, inFile);
			else
			{
				// /*debug*/ std::cout << "errorCheckConfig " << word << std::endl; //throw invalid directive
				throw (std::invalid_argument(ERR_DIRECTIVEINVALID));
			}
		}
		catch (std::exception &err)
		{
			std::cout << "pika" << std::endl;
			throw ;
		}
	}
	/* restore original position after error checks */
	inFile.clear();
	inFile.seekg(ori_pos);
	return (0);
}
