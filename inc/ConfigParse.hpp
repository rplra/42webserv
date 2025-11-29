#ifndef __CONFIGPARSE_HPP__
# define __CONFIGPARSE_HPP__

# include <fstream> 
# include <sstream> 

enum e_common_directive
{
	ROOT,
	INDEX,
	AUTOINDEX,
	ERROR_PAGE,
	CLIENT_MAX_BODY_SIZE,
};

enum e_global_scope
{
	SERVER
	/* plus common directives */
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
	ALLOWED_METHODS,	//allowed_methods
	UPLOAD_STORE,		// upload path
	RETURN				// redirect path
	/* ... plus common directives */
};

#endif