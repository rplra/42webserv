NAME		=	webserv

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -Iinc #-fsanitize=address -g3
RM			=	rm -rf

# Directory
SRC			=	src
OBJ			=	obj
PARSE_DIR	=	$(SRC)/parsing

# Sources
PARSE_FILES	=	ParseConfig.cpp ParseLocation.cpp ParseUtils.cpp \
				ParseErrorCheck.cpp ParseErrorCheckType.cpp ParseErrorCheckLoc.cpp
SRC_FILES	=	main.cpp Cgi.cpp Config.cpp Cookie.cpp Utils.cpp Request.cpp Response.cpp ServerManager.cpp Client.cpp

SRCS		=	$(addprefix $(SRC)/, $(SRC_FILES))			\
				$(addprefix $(PARSE_DIR)/, $(PARSE_FILES))

OBJS		=	$(patsubst %.cpp, $(OBJ)/%.o, $(SRCS))


all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "Compile $(NAME)    : OK!"

$(OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ)
	@echo "Clean $(NAME)      : OK!"

fclean: clean
	@$(RM) $(NAME)
	@echo "Full Clean $(NAME) : OK!"

re: fclean all

leaks:
	leaks --atExit -- ./$(NAME)

valgrind: 
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) 

.PHONY: all clean fclean re leaks valgrind
