NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I Includes

SRC_DIR = src
HEAP_DIR = $(SRC_DIR)/heap
TIME_DIR = $(SRC_DIR)/time
CODERS_DIR = $(SRC_DIR)/coders

SRCS = main.c \
	$(HEAP_DIR)/heap.c \
	$(HEAP_DIR)/heap_ops.c \
	$(TIME_DIR)/time_utils.c \
	$(TIME_DIR)/logger.c \
	$(CODERS_DIR)/coder.c \
	$(CODERS_DIR)/coder_utils.c \
	$(CODERS_DIR)/monitor.c \
	$(CODERS_DIR)/dongle.c \
	$(CODERS_DIR)/dongle_utils.c \
	$(CODERS_DIR)/simulation.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re