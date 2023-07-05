#						 #
#--- GLOBAL VARIABLES ---#
#						 #

NAME		=	webserv

CC				=	c++
MKDIR			=	mkdir -p
HEADER_DIR		=	./includes/
OBJECTS_DIR		=	./obj/
SOURCES_DIR		=	./src/

IFLAGS		=	-I $(HEADER_DIR)
CFLAGS		=	-std=c++98 -Werror -Wextra -Wall $(IFLAGS)

#------Colors--------
BLACK	=	"\033[1;30m"
RED		=	"\033[1;31m"
BGREEN	=	"\033[1;32m"
GREEN	=	"\033[0;32m"
YELLOW	=	"\033[1;33m"
PURPLE	=	"\033[1;35m"
CYAN	=	"\033[1;36m"
SCYAN	=	"\033[0;36m"
WHITE	=	"\033[1;37m"
EOC		=	"\033[0;0m"
#====================

#					  #
#--- Program Files ---#
#					  #

FILES	=	\
			Cgi \
			Client \
			configCheck \
			main \
			Response \
			SocketConfig \
			Socket \
			WebServer

#------Paths---------
SOURCES		=	$(addprefix $(SOURCES_DIR),$(addsuffix .cpp,$(FILES)))
OBJECTS		=	$(addprefix $(OBJECTS_DIR),$(addsuffix .o,$(FILES)))
#====================

#					 #
#--- GLOBAL RULES ---#
#					 #

$(NAME): $(OBJECTS)
	@$(CC) $(CFLAGS) $(SOURCES) -o $@
	@echo $(BGREEN) "WEBSERV COMPILED" $(EOC)

$(OBJECTS_DIR)%.o : $(SOURCES_DIR)%.cpp
	@$(MKDIR) $(OBJECTS_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
	@rm -rf $(OBJECTS_DIR)
	@echo $(YELLOW) "OBJECT FILES CLEANED" $(EOC)

fclean: clean
	@rm -f $(NAME)
	@echo $(RED)" EXECUTABLE DELETED" $(EOC)

re: fclean all

.PHONY: all clean fclean make
