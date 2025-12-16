#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "Webserv.hpp"
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include "Debug.hpp"

# define CLIENT_MAX_BODY		1000
class Request;

struct Location
{
	std::string					_path;
	std::string					_root;
	std::string					_index;
	bool						_autoindex;
	size_t						_client_max_body_size;
	std::map<int, std::string>	_error_pages;
	std::vector<std::string> 	_allowed_methods;

	std::string					_upload_path;
	std::map<int, std::string>	_cgi;
	std::map<int, std::string>	_redirect;
};

class Server
{
public:
	Server();
	~Server() {};

	void		parseServer(std::ifstream &inFile);
	void		printServer();
	void		printErrorPage();
	void		assignMapContainer(std::map<int, std::string> &data, std::istringstream &iss);
	void		assignVecContainer(std::vector<std::string> &data, std::istringstream &iss);
	void		assignCgiContainer(std::map<int, std::string> &data, std::istringstream &iss);


	// getters - these method names must align for ALL otherwise integration fails
	// HTTP use    : getRoot(), getIndex(), getLocations(), getErrorPages()
	// Network use : getHost(), getPort(),  getServerNames()

	// helpers
	// HTTP use    : matchLocation() - returns Location* based on longest prefix match
	//			   : isAllowedMethod()
	// Network use : isMatchesPort() (optional) - does this server listen on given port?
	//			   : isMatchesHost() (optional) - does this server match the Host header?

// this is where both network and config MUST ALIGN in terms of what DATA TYPE and FIELDS to have
// network must let config know what fields it expects (if config doesnt have, then it fails to build)

	const std::string&					getHost() const;
	int									getPort() const;
	const std::vector<std::string>&		getServerNames() const;
	const std::string&					getRoot() const;
	const std::string&					getIndex() const;
	bool								getAutoindex() const;
	size_t								getClientMaxBodySize() const;
	std::string							getFullPath(const Request& request) const;
	std::vector<Location>&				getLocations();
	const std::vector<Location>&		getLocations() const;
	const std::string					getErrorPagePath(int errorCode) const;
	const Location*						getMatchingLocation(const std::string& requestPath) const;

	// setters
	void								setPort(int port);
	void								setHost(const std::string& host);
	void								addServerName(const std::string& server_name);
	void								setRoot(const std::string& root);
	void								setIndex(const std::string& index);
	void								setAutoindex(bool autoindex);
	void								setClientMaxBodySize(size_t size);
	void								addErrorPage(int errorCode, const std::string& path);
	void								addLocation(const Location& location);

private:
	std::string					_host;
	int							_port;
	std::vector<std::string>	_server_names;
	
	// Server-Wide Default (used by HTTP when location-specific value not present)
	std::string					_root;
	std::string					_index;
	bool						_autoindex;
	size_t						_client_max_body_size;

	// HTTP use
	std::map<int, std::string>	_error_pages;
	std::vector<Location>		_locations;

	// methods : parser funcs
	template <typename T>
	bool		checkCommonDirective(std::string str, std::istringstream &iss, T &data);
	template <typename T>
	void		parseCommonDirective(std::size_t code, std::istringstream &iss, T &data);

	void		parseServerDirective(std::size_t code, std::istringstream &iss, std::ifstream &inFile);
	void		parseLocationDirective(std::size_t code, std::istringstream &iss, Location &data);
	bool		handleServerDirective(std::string str, std::istringstream &iss, std::ifstream &inFile);
	bool		handleLocationDirective(std::string str, std::istringstream &iss, Location& data);
	void		parseListen(std::istringstream &iss);
	void		initLocation(Location &obj);
	void		parseLocation(std::ifstream &inFile, std::istringstream &iss);
};

// this is MAIN BRIDGE btw Network + Config 
// Network depends on Config to know which Server obj's exist, which port they are on,
// and which servers share the same port (virtual hosts)
// HTTP depends on Server obj inside Config for routing + request handling
class Config
{
public:
	Config();
	~Config() {};

	// void	parseConfig(char **av);

	// NON CONST for parser to update this class while parsing
	void		printAllServer();

	std::vector<Server>& 			getServers();
	std::vector<Server*>&			getServerOnPort(int port);

	const std::vector<Server>& 		getServers() const;
	const std::vector<Server*>&		getServerOnPort(int port) const;

private: 
	// error check use
	// errCheck								_check;
	
	std::vector<Server>						_servers;
	std::map<int, std::vector<Server*> >	_port_map;
	
	// // helper functions

	// // parser: error checks
	// void		startParser(std::ifstream &inFile);

	// bool		isDirective(const std::string &word);
	// int			countArgs(std::istringstream &iss);
	// bool		checkBraces(const std::string &to_find, std::vector<std::string> &data);
	// bool		noMoreBrace(std::istringstream &iss);
	// bool		checkTrimSemicolon(std::string &buffer);
	// bool		ignoreKeyword(std::string &word, std::istringstream &iss, errCheckGroup &data);
	
	// bool		errorCheckConfig(std::ifstream &inFile);
	// void		errorCheckServer(std::istringstream &iss, std::ifstream &inFile);
	// void		errorCheckLocation(std::istringstream &iss, std::ifstream &inFile);
	// bool		errorServerDirective(std::string &str, std::istringstream &iss, std::ifstream &inFile);
	// bool		errorCommonDirective(std::string &str, std::istringstream &iss, errCheckGroup &data);
	// bool		errorLocationDirective(std::string str, std::istringstream &iss);

	// void		checkValidTypeCommon(size_t code, std::istringstream &iss, errCheckGroup &data);
	// void		checkValidTypeLoc(size_t code, std::istringstream &iss);
	// void		checkValidTypeUpload(std::istringstream &iss);
	// void		checkValidTypeCgi(std::istringstream &iss);
	// void		checkValidTypeRoot(std::istringstream &iss, errCheckGroup &data);
	// void		checkValidTypeIndex(std::istringstream &iss, errCheckGroup &data);
	// void		checkValidTypeErrPage(std::istringstream &iss, errCheckGroup &data);
	// void		checkValidTypeMaxBodySize(std::istringstream &iss);
	// void		checkValidTypeAutoindex(std::istringstream &iss);
	// void		checkValidTypeAllowed(std::istringstream &iss);
	
	// bool		checkMatch(const char* types[], std::string word, const std::string err_message);
	// bool		checkDuplicate(std::string &str, std::vector<std::string> &data, const std::string err_message);
	// bool		checkDuplicateCgi(std::string &str, std::istringstream &iss, std::map<std::string, std::string> &data);
	// void		checkServerArgCount(size_t code, std::istringstream &iss);
	// void		checkCommonArgCount(size_t code, std::istringstream &iss);
	// void		checkLocationArgCount(size_t code, std::istringstream &iss);
	// void		errorLocationBase(std::istringstream &iss);
	// bool		errorServerBase(std::istringstream &iss);
};

#endif