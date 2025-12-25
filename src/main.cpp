#include "Webserv.hpp"
#include "ConfigParse.hpp"

int g_signal;

int main(int ac, char **av)
{
	try
	{
		checkArgument(ac);

		g_signal = 1;
		signal(SIGINT, handleSignal);

		Config          config;
		ConfigParser    parser(config);
		parser.parseConfig(av);

		ServerManager webserv(config);
		webserv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << "Exception: " << e.what() << RESET << std::endl;
		return (1);
	}

    return (0);
}
