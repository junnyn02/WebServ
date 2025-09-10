CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -g3 -std=c++98

NAME = webserv

SRCS := main.cpp serverCore.cpp

OBJDIR = objects/
INCDIR	= includes/

OBJS = $(addprefix $(OBJDIR), $(SRCS:.cpp=.o))

INCLUDE = -I $(INCDIR)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(NAME) $(OBJS)

$(OBJDIR)%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re