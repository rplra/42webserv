// #ifndef __CONFIGPARSER_HPP__
// # define __CONFIGPARSER_HPP__

// # include "Webserv.hpp"
// class Config;

// /* 
// 	directives that can appear in multiple scopes (server / location)
// 	eg; root, index, autoindex, error_page, client_max_body_size can appear at server / location level
// */
// enum e_common_directive
// {
// 	NONE_COMMON,
// 	ROOT,
// 	INDEX,
// 	AUTOINDEX_DIRECTIVE,
// 	ERROR_PAGE,
// 	CLIENT_MAX_BODY_SIZE,
// };

// /* 
// 	tracks global parsing context
// 	NONE GLOBAL	: not inside any server block
// 	SERVER		:  inside a server block
// 	determines whether directive belongs to server or invalid globally
//  */
// enum e_global_scope
// {
// 	NONE_GLOBAL,
// 	SERVER
// 	/* plus common directives */
// };

// /* 
// 	tracks current parsing scope inside a server block
// 	NONE_SERVER : not inside a location yet
// 	LISTEN		: parsing a listen directive
// 	SERVER_NAME	: parsing server_name
// 	LOCATION	: parsing a location {...} block
// 	ensures directives go to the right obj: server / location level
// */
// enum e_server_scope
// {
// 	NONE_SERVER,
// 	LISTEN,
// 	SERVER_NAME,
// 	LOCATION
// 	/* ... plus common directives */
// };

// /* 
// 	tracks parsing inside a location block
// 	NONE_LOCATION	: not currently parsing a specific location directive
// 	CGI_HANDLER		: parsing cgi config
// 	ALLOWED_METHODS	: parsing allowed methods
// 	UPLOAD_STORE	: parsing upload directory
// 	RETURN			: parsing return / redirect directive
// */
// enum e_location_scope
// {
// 	NONE_LOCATION,
// 	CGI_HANDLER,
// 	ALLOWED_METHODS,
// 	UPLOAD_STORE,
// 	RETURN
// 	/* ... plus common directives */
// };


// class ConfigParser
// {
// public:
// 	ConfigParser(Config &config);
// 	~ConfigParser() {};

// 	void	parseConfig(std::string &filename); // to update
// 	// void	parseServer(std::ifstream &inFile); 
// 	// void	assignMapContainer(std::map<int, std::string> &data, std::istringstream &iss);
// 	// void	assignVecContainer(std::vector<std::string> &data, std::istringstream &iss);

// 	// debug funcs
// 	// void	printErrorPage();
// 	// void	printServer();

// private:
// 	Config&	_config;
// 	int 	_global_scope;
// 	int		_server_scope;
// 	// int		_location_scope;
// 	// int		_common_directive;

// 	// void	startParser(std::ifstream &inFile);

// 	// template <typename T>
// 	// bool	checkCommonDirective(std::string str, std::istringstream &iss, T &data);
// 	// template <typename T>
// 	// void	parseCommonDirective(std::size_t code, std::istringstream &iss, T &data);

// 	// bool	checkServerDirective(std::string str, std::istringstream &iss, std::ifstream &inFile);
// 	// bool	checkLocationDirective(std::string str, std::istringstream &iss, Location& data);
	
// 	// void	parseServerDirective(std::size_t code, std::istringstream &iss, std::ifstream &inFile);
// 	// void	parseLocationDirective(std::size_t code, std::istringstream &iss, Location &data);
// 	// void	parseListen(std::istringstream &iss);
// 	// void	initLocation(Location &obj);

// 	// void	parseLocation(std::ifstream &inFile, std::istringstream &iss);

// 	// void			printAllServer();
// 	void			block_validation(std::string buffer);	// temp - to replace
// 	std::string		removeSemicolon(std::string str);		// temp - to replace
// };

// #endif