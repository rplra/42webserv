#ifndef __CONFIGPARSE_HPP__
# define __CONFIGPARSE_HPP__

# include <fstream> 
# include <sstream> 
# include "Config.hpp"

enum e_common_directive
{
	ROOT,
	INDEX,
	AUTOINDEX_DIR,
	ERROR_PAGE,
	CLIENT_MAX_BODY_SIZE,
};

enum e_global_scope
{
	SERVER
	/* plus common directives */
};

enum cgi
{
	PY,
	CPP,
	JS,
	PHP
};

enum e_server_scope
{
	LISTEN,
	SERVER_NAME,
	LOCATION
	/* ... plus common directives */
};

enum e_location_scope
{
	CGI_HANDLER,		// not for global scope
	ALLOWED_METHODS,	// allowed_methods
	UPLOAD_STORE,		// upload path
	RETURN,				// redirect path
	ALIAS				// alias to replace root
	/* ... plus common directives */
};

struct errCheckGroup
{
	std::vector<std::string>			dup;
	std::vector<std::string>			brace;
	bool								b_openBrace;
	bool								b_closeBrace;
	std::map<std::string, std::string>	cgi;
	std::string							root;
};

struct errCheckPortName
{
	// std::map<std::string, std::string>	hostPort;
	std::string							host;
	std::string							port;
	std::vector<std::string>			server_names;
};

struct errCheck
{
	int												line_count;
	std::string										keyword;
	errCheckGroup									loc;
	errCheckGroup									serv;
	std::vector<errCheckPortName>					portNameMap;
};

class ConfigParser
{
public:
	ConfigParser(Config& target);
	void	parseConfig(char **av);

private:
	ConfigParser();
	Config&			_config;
	errCheck		_check;
	// helper functions
	// void		printAllServer();

	// parser: error checks
	void		startParser(std::ifstream &inFile);

	bool		isDirective(const std::string &word);
	int			countArgs(std::istringstream &iss);
	bool		checkBraces(const std::string &to_find, std::vector<std::string> &data);
	bool		noMoreBrace(std::istringstream &iss);
	bool		checkTrimSemicolon(std::string &buffer);
	bool		ignoreKeyword(std::string &word, std::istringstream &iss, errCheckGroup &data);
	void		errorParseListen(std::istringstream &iss, errCheckPortName &tmp);
	void		errorParseServerName(std::istringstream &iss, errCheckPortName &tmp);

	
	bool		errorCheckConfig(std::ifstream &inFile);
	void		errorCheckServer(std::istringstream &iss, std::ifstream &inFile, errCheckPortName &tmp);
	void		errorCheckLocation(std::istringstream &iss, std::ifstream &inFile);
	bool		errorServerDirective(std::string &str, std::istringstream &iss, std::ifstream &inFile, errCheckPortName &tmp);
	bool		errorCommonDirective(std::string &str, std::istringstream &iss, errCheckGroup &data);
	bool		errorLocationDirective(std::string str, std::istringstream &iss);

	void		checkValidTypeServer(size_t code, std::istringstream &iss);
	void		checkValidTypeCommon(size_t code, std::istringstream &iss, errCheckGroup &data);
	void		checkValidTypeLoc(size_t code, std::istringstream &iss);
	void		checkValidTypeListen(std::istringstream &iss);
	void		checkValidTypeAlias(std::istringstream &iss);
	void		checkValidTypeUpload(std::istringstream &iss);
	void		checkValidTypeCgi(std::istringstream &iss);
	void		checkValidTypeRoot(std::istringstream &iss, errCheckGroup &data);
	void		checkValidTypeIndex(std::istringstream &iss, errCheckGroup &data);
	void		checkValidTypeErrPage(std::istringstream &iss, errCheckGroup &data);
	void		checkValidTypeMaxBodySize(std::istringstream &iss);
	void		checkValidTypeAutoindex(std::istringstream &iss);
	void		checkValidTypeAllowed(std::istringstream &iss);
	
	bool		checkMatch(const char* types[], std::string word, const std::string err_message);
	bool		checkDuplicate(std::string &str, std::vector<std::string> &data, const std::string err_message);
	void		checkDuplicateEndpoint(size_t code, std::istringstream &iss, errCheckPortName &tmp);
	bool		checkDuplicateServerName(std::vector<std::string> &master, std::vector<std::string> &to_find);
	// bool		checkDuplicateEntry(std::vector<std::string> &data, std::vector<std::string> &content);
	bool		hasDuplicateHostPort(errCheckPortName &tmp);

	bool		checkAliasRootConflict(std::string &str, std::vector<std::string> &data);
	bool		checkDuplicateCgi(std::string &str, std::istringstream &iss, std::map<std::string, std::string> &data);
	void		checkServerArgCount(size_t code, std::istringstream &iss);
	void		checkCommonArgCount(size_t code, std::istringstream &iss);
	void		checkLocationArgCount(size_t code, std::istringstream &iss);
	void		errorLocationBase(std::istringstream &iss);
	bool		errorServerBase(std::istringstream &iss);

};

#endif