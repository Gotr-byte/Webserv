NAME := Webserv
SRCFILES := main.cpp Configuration.cpp

CC = c++
CFLAGS = -std=c++98 -Wextra -Werror -Wall
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