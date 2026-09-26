NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I Includes
CFLAGS_DEBUG = -Wall -Wextra -Werror -pthread -I Includes -g -O0
CFLAGS_ASAN = -Wall -Wextra -Werror -pthread -I Includes -g -O0 -fsanitize=address -fno-omit-frame-pointer
CFLAGS_TSAN = -Wall -Wextra -Werror -pthread -I Includes -g -O0 -fsanitize=thread -fno-omit-frame-pointer

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
	$(CODERS_DIR)/simulation.c \
	src/args.c

OBJS = $(SRCS:.c=.o)
OBJS_DEBUG = $(SRCS:.c=.debug.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(NAME)

asan: CFLAGS = $(CFLAGS_ASAN)
asan: $(NAME)

tsan: CFLAGS = $(CFLAGS_TSAN)
tsan: $(NAME)

clean:
	rm -f $(OBJS) $(OBJS_DEBUG)

fclean: clean
	rm -f $(NAME)

re: fclean all

# Valgrind memory leak detection
# --leak-check=full: detailed leak report
# --show-leak-kinds=all: show all leak types (definite, indirect, possible, reachable)
# --track-origins=yes: show origin of uninitialized values
# --error-exitcode=1: exit with code 1 on error (useful for CI)
# --suppressions: suppress known false positives (create if needed)
VG_MEM_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 --verbose

# Valgrind helgrind (thread errors: data races, deadlocks, lock order violations)
# --history-level=full: detailed stack traces
# --conflict-cache-size=8: increase for complex programs
VG_HEL_FLAGS = --tool=helgrind --history-level=full --conflict-cache-size=8 --error-exitcode=1

# Test arguments (adjust as needed)
TEST_ARGS = 3 10000 200 200 200 3 100 fifo

valgrind-mem: $(NAME)
	valgrind $(VG_MEM_FLAGS) ./$(NAME) $(TEST_ARGS)

valgrind-hel: $(NAME)
	valgrind $(VG_HEL_FLAGS) ./$(NAME) $(TEST_ARGS)

valgrind: valgrind-mem valgrind-hel

# AddressSanitizer (memory errors: buffer overflow, use-after-free, leaks)
asan-test: asan
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 ./$(NAME) $(TEST_ARGS)

# ThreadSanitizer (data races, deadlocks)
tsan-test: tsan
	TSAN_OPTIONS=halt_on_error=1 ./$(NAME) $(TEST_ARGS)

# Run basic functional test
test: $(NAME)
	@echo "=== Testing FIFO ==="
	./$(NAME) 2 5000 100 100 100 2 50 fifo
	@echo ""
	@echo "=== Testing EDF ==="
	./$(NAME) 2 5000 100 100 100 2 50 edf
	@echo ""
	@echo "=== Testing burnout ==="
	./$(NAME) 3 500 200 200 200 10 100 edf
	@echo ""
	@echo "=== Testing single coder ==="
	./$(NAME) 1 5000 200 200 200 3 100 fifo

.PHONY: all clean fclean re debug asan tsan valgrind valgrind-mem valgrind-hel asan-test tsan-test test