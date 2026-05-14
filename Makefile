NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRCDIR	= src
INCDIR	= include

SRCS	= $(SRCDIR)/main.c \
		  $(SRCDIR)/parse.c \
		  $(SRCDIR)/simulation.c \
		  $(SRCDIR)/coder.c \
		  $(SRCDIR)/coder_state.c \
		  $(SRCDIR)/dongle.c \
		  $(SRCDIR)/heap.c \
		  $(SRCDIR)/heap_sift.c \
		  $(SRCDIR)/monitor.c \
		  $(SRCDIR)/log.c \
		  $(SRCDIR)/run.c \
		  $(SRCDIR)/utils.c

OBJS	= $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
