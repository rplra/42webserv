#ifndef __COOKIE_HPP__
# define __COOKIE_HPP__

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

class Cookie
{
private:
	Cookie() {};
public:
	static std::string		setRandCookie();
};

#endif