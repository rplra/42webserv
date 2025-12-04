#ifndef __MACROS_HPP__
#define __MACROS_HPP__

#include <string>

// colours
const std::string GREEN  = "\033[38;2;168;204;124m";
const std::string RED    = "\033[38;2;191;97;106m";
const std::string CYAN   = "\033[38;2;136;193;208m";
const std::string PURPLE = "\033[38;2;174;134;255m";
const std::string PINK   = "\033[38;2;224;147;217m";
const std::string YELLOW = "\033[38;2;255;214;2m";
const std::string ORANGE = "\033[38;2;255;135;0m";
const std::string GREY   = "\033[90m";
const std::string RESET  = "\033[0m";

// errors
const std::string ERR_ARGFORMAT = "Invalid argument. Usage: ./webserv [configuration file]";
const std::string ERR_FILEINVALID = "Invalid file!";
const std::string ERR_FILEEMPTY = "Empty file!";
// errors config
const std::string ERR_DIRECTIVEINVALID = "Unknown directive found!";
const std::string ERR_ARGCOUNTINVALID = "Invalid number of arguments!";
const std::string ERR_SEMICOLONMISSING = "Directive is not terminated by ';'";

#endif