#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

/* check if string ends with semicolon */
// void	Server::checkSemicolon(std::istringstream &iss)
// {

// }

// void	Server::checkSemicolon(std::istringstream &iss)
// {
// 	std::istringstream	iss_cpy(iss.str());
// }

bool Config::ignoreKeyword(std::string &word)
{
	const char *arr[] =
	{
		"#",
		"{",
		"}",
		NULL
	};

	for (size_t i=0;  arr[i];  i++)
	{
		if (word == arr[i])
		{
			std::cout << "ignoreKeyword: " << word << std::endl;
			return (1);
		}
	}
	return (0);
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

	/*debug*/std::cout << ", common_arg_count: " << count << std::endl;
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
	// if (code == ERROR_PAGE && count == 2)
	// 	return ;
	// else if (count == 1)
	// 	return ;
	throw (std::invalid_argument(ERR_ARGCOUNTINVALID));
}

void	Config::checkServerArgCount(size_t code, std::istringstream &iss, std::ifstream &inFile)
{
	(void) code;
	(void) inFile;
	std::string word;
	int			count = 0;

	if (code != LOCATION)
		count = countArgs(iss);

	/*debug*/std::cout << ", serv_arg_count: " << count << std::endl;
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
		
		case LOCATION:
		if (!errorLocationArgCount(iss, inFile)) //handle loc arg c
			return ;
		break;
	}
	throw std::invalid_argument(ERR_ARGCOUNTINVALID);
}

/* return (0) == no error */
bool	Config::errorServerDirective(std::string &str, std::istringstream &iss, std::ifstream &inFile)
{
	(void) inFile;

	if (str == "}")
		return (0);

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
				checkServerArgCount(i, iss, inFile);
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
bool	Config::errorCommonDirective(std::string &str, std::istringstream &iss)
{
	if (str == "}")
		return (0);

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
			std::cout << str << std::endl;
			try
			{
				checkCommonArgCount(i, iss);
			}
			catch (std::exception &err)
			{
				std::cout << RED << "errorCommonDirective: " << RESET << std::endl;
				throw ;
			}
			/* check semicolon : end and no other chars */
			return (0);
		}
	}
	std::cout << PINK << "errorCommonDirective invalid: " << str << RESET << std::endl;
	return (1); //return 1 if type_not_found
}

void	Config::errorCheckServer(std::ifstream &inFile)
{
	std::string			word, buffer;
	std::streampos		pos = inFile.tellg();
	std::istringstream	iss;

	while (std::getline(inFile, buffer))
	{
		this->_check.line_count++;

		iss.clear();
		iss.str(buffer);
		/*debug*/ std::cout << YELLOW << buffer << RESET << std::endl;

		if (!(iss >> word) || ignoreKeyword(word))
		{
			pos = inFile.tellg(); //tellg returns end of readed line
			continue ;
		}
		this->_check.keyword = word;
		/*debug*/ std::cout << "iss: " << word << std::endl;
		if (word == "server")
		{
			inFile.seekg(pos);	//rewind back
			break;
		}
		// pos = inFile.tellg();

		try
		{
			if (errorCommonDirective(word, iss) && errorServerDirective(word, iss, inFile))
				throw (std::invalid_argument(ERR_DIRECTIVEINVALID)); //invalid_directive
		}
		catch (std::exception &err)
		{
			std::cout << "errorCheckServer" << std::endl;
			throw ;
		}
	}
}

bool	Config::errorCheckConfig(std::ifstream &inFile)
{
	std::streampos		ori_pos = inFile.tellg();
	std::string			word, buffer;
	std::istringstream	iss;

	/* errorCheckScope */
	// implementation ...

	/* errorCheckServer */
	while (std::getline(inFile, buffer))
	{
		this->_check.line_count++;
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		/*debug*/ std::cout << "errorCheckConfig_word: " << word << std::endl; //throw invalid directive

		try
		{
			this->_check.keyword = word;
			if (word == "server")
				this->errorCheckServer(inFile);
			else
			{
				/*debug*/ std::cout << "errorCheckConfig " << word << std::endl; //throw invalid directive
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
