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

san: fclean
	$(CC) $(CFLAGS) -fsanitize=thread $(SRCS) -I$(INCDIR) -o $(NAME)_san

run: $(NAME)
	-@./$(NAME) 1 800 200 200 200 3 50 fifo
	-@./$(NAME) 5 800 200 200 200 3 50 fifo
	-@./$(NAME) 5 800 200 200 200 3 50 edf
	-@./$(NAME) 5 200 800 200 200 3 50 fifo
	-@./$(NAME) 2 800 200 200 200 3 50 fifo

fclean: clean
	rm -f $(NAME) $(NAME)_san

re: fclean all

.PHONY: all clean fclean re san run
