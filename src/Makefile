NAME := Webserv
INC_DIR = ../includes
CC = c++
# CFLAGS = -std=c++98 -Wextra -Werror -Wall -I$(INC_DIR) -g -fsanitize=address
CFLAGS = -g -fsanitize=address
SRCFILES := $(shell find . -iname "*.cpp")
OBJS = $(SRCFILES:.cpp=.o)

%o: %cpp
	$(CC) $(CFLAGS) -c $(SRCFILES)
all: $(NAME)


$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re:		fclean all

.PHONY: clean fclean re