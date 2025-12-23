#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// void replaceAll(std::string &str,
//                 const std::string &from,
//                 const std::string &to) {
//     size_t pos = 0;
//     while ((pos = str.find(from, pos)) != std::string::npos) {
//         str.replace(pos, from.length(), to);
//         pos += to.length();
//     }
// }

void	findReplace(std::string &buffer, std::string to_find, std::string &to_replace)
{
	size_t pos = 0;

	while (buffer.find(to_find, pos) != std::string::npos)
	{
		pos = buffer.find(to_find, pos);
		buffer.replace(pos, to_find.length(), to_replace);
		pos += 1;
	}
}

int	main(int ac, char **av)
{
	if (ac != 4)
	{
		std::cerr << "Input must be 3: <error_code> <title> <content>" << std::endl;
		return (0);
	}
	std::string title, content;
	content = av[3];
	title = av[1];
	title += ' ';
	title += av[2];

	std::ifstream infile("template.html");
	if (!infile.is_open())
	{
		std::cerr << "File does not exist!" << std::endl;
		return (1);
	}

	std::string		filename = av[1];
	filename += ".html";
	std::ofstream	dest_file(filename);
	if (!dest_file.is_open())
	{
		std::cerr << "Failed to create file!" << std::endl;
		return (1);
	}

	/* copy replace content */
	std::string	buffer;
	while (std::getline(infile, buffer))
	{
		findReplace(buffer, "{{TITLE}}", title);
		findReplace(buffer, "{{DESC}}", content);
		buffer += '\n';
		dest_file << buffer;
	}
	dest_file.close();
	std::cout << "File " << av[1] << ".html file is created successfully!" << std::endl;
	return (0);
}