NAME		:=	webserv

CXX			:=	c++

CXXFLAGS	:=	-Wall -Wextra -Werror -g3 -std=c++98
SANFLAGS	:=	-fsanitize=address,undefined

OBJ_PATH	:=	objects/
INC_PATH	:=	include/
SRC_PATH	:=	srcs/

SRC			:=	main.cpp 
SRC			+=	serverCore.cpp
SRC			+=	Request.cpp
SRC			+=	ResponseBuilder.cpp
SRC			+=	utils.cpp

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

asan:	CXXFLAGS += $(SANFLAGS)
asan:	fclean $(NAME)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re