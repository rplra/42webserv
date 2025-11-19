#include "Webserv.hpp"

int main (int ac, char **av)
{
	// (void) av;
	// (void) ac;
	checkArgument(ac);

	/* read the av file */
	/* do not read/write without poll, this for testing only */
	parseConfig(av);

	return (0);
}