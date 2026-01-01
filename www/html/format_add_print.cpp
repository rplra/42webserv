#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <locale>

std::string  removeSpaces(std::string &str)
{
	size_t i=0;

	for (i=0; i < str.length(); i++)
	{
		if (!std::isspace(str[i]))
			break ;
	}
	return (&str[i]);
}

/* adds print() to each line in html file for python to serve page */
int main(int ac, char **av)
{
	if (ac != 3)
		std::cerr << "Input must be 2: <input_filename> <output_filename>" << std::endl;

	std::ifstream   infile(av[1]);
	if (!infile)
	{
		std::cerr << "Error opening input file" << std::endl;
		return (1);
	}
	std::ofstream   outfile(av[2]);
	if (!outfile)
	{
		std::cerr << "Error creating output file" << std::endl;
		return (1);
	}
	
	std::string 		buffer, word;
	std::istringstream	iss;

	while (std::getline(infile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		if (word == "/*" || word == "<!--")
			continue ;
		// outfile << "print(\"" << buffer << "\")" << std::endl;
		// outfile << "print(\"" << removeSpaces(buffer) << "\")" << std::endl;
		outfile << "echo \"" << removeSpaces(buffer) << "\"" << std::endl;
	}

	infile.close();
	outfile.close();
	return (0);
}