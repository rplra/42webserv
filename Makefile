NAME		=	webserv

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -Iinc
# CXXFLAGS	+=	-fsanitize=address -g3
RM			=	rm -rf

# Directory
SRC			=	src
OBJ			=	obj

# Sources
SRCS        =	src/main.cpp \
				src/Config.cpp \
				src/ConfigParse.cpp \
				src/Utils.cpp \
				src/Request.cpp \
				src/Response.cpp \
				src/ServerManager.cpp \
				src/Client.cpp

OBJS        = 	$(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "Compile $(NAME)    : OK!"

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(OBJ)
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
