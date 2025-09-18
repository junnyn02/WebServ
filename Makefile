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