#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <locale>

/*
   This is a helper function that adds print() / echo
   to each line in html file for python/php formatting
 * ************************************************************** */

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

int main(int ac, char **av)
{
	if (ac != 4)
	{
		std::cerr << "Input must be 3: <input_filename> <output_filename> <mode:py or php>" << std::endl;
		return (1);
	}

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
	
	std::string 		buffer, word, mode;
	std::istringstream	iss;

	mode = av[3];
	while (std::getline(infile, buffer))
	{
		iss.clear();
		iss.str(buffer);
		if (!(iss >> word))
			continue ;
		if (word == "/*" || word == "<!--")
			continue ;
			
		if (mode == "py")
			outfile << "print(\"" << buffer << "\")" << std::endl;
		else if (mode == "php")
			outfile << "echo \"" << removeSpaces(buffer) << "\";" << std::endl;
	}

	infile.close();
	outfile.close();
	return (0);
}