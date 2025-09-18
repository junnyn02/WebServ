NAME		:=	webserv

CXX			:=	c++

CXXFLAGS	:=	-Wall -Wextra -Werror -g3 -std=c++98

OBJ_PATH	:=	objects/
INC_PATH	:=	include/
SRC_PATH	:=	srcs/

SRC			:=	main.cpp 
SRC			+=	serverCore.cpp

OBJ			:=	$(SRC:.cpp=.o)

SRCS		:=	$(addprefix $(SRC_PATH), $(SRC))

OBJS 		:=	$(addprefix $(OBJ_PATH), $(OBJ))

INCLUDE		:=	-I $(INC_PATH)

all: $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(NAME) $(OBJS)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

NAME = Webserv

SRC_PATH = srcs/

SRC = main.cpp Request.cpp

SRCS = ${addprefix $(SRC_PATH), $(SRC)}

OBJDIR = objects

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

INCLUDE = -I include/

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(NAME) $(OBJS)

$(OBJDIR)/%.o:  $(SRC_PATH)%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re