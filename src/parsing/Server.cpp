#include "Utils.hpp"
#include "Config.hpp"
#include "ConfigParse.hpp"

Server::Server()
{
	this->_port									= -1;
	this->_autoindex							= 0;
	this->_client_max_body_size					= CLIENT_MAX_BODY;	//default value
}

Server::~Server()
{

}