NAME		=	webserv

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -Iinc #-fsanitize=address -g3
RM			=	rm -rf

# Directory
SRC			=	src
OBJ			=	obj

# Sources
<<<<<<< HEAD
PARSE_FILES	=	ParseConfig.cpp ParseUtils.cpp Server.cpp
SRC_FILES	=	main.cpp Utils.cpp

# SRCS		=	src/main.cpp
# 				src/Utils.cpp
# OBJS		=	$(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)

SRCS		=	$(addprefix $(SRC)/, $(SRC_FILES))			\
				$(addprefix $(PARSE_DIR)/, $(PARSE_FILES))

OBJS		=	$(patsubst %.cpp, $(OBJ)/%.o, $(SRCS))

=======
SRCS        =	src/main.cpp \
				src/Request.cpp \
				src/ConfigParsing.cpp \
				src/ServerParsing.cpp \
				src/ResponseHandling.cpp \
				src/SocketHandling.cpp \
				src/RequestHandling.cpp \
				src/Utils.cpp 

OBJS        = 	$(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)
>>>>>>> origin/socket-natalie

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
