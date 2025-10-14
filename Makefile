NAME        := ft_ping
SRCS_DIR    := src
OBJ_DIR     := obj
INC_DIR     := include

CC            := cc
CFLAGS        := -Wall -Wextra -Werror
DEBUG_FLAGS   := -g3 -O0 -fsanitize=address
RELEASE_FLAGS := -O2
LIBS          := #-lpcap -lpthread
MAKEFLAGS     := -j

SRCS        := $(wildcard $(SRCS_DIR)/*.c)
OBJS        := $(patsubst $(SRCS_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS        := $(OBJS:.o=.d)   # For dependency tracking

BUILD_MODE  ?= DEBUG 

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += $(DEBUG_FLAGS)
    BUILD_MSG := DEBUG mode with -g3 -O0 -fsanitize=address
else
    CFLAGS += $(RELEASE_FLAGS)
    BUILD_MSG := RELEASE mode with -O2
endif

.PHONY: all clean fclean re debug release run gdb

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)
	@echo " Build completed: $(NAME) ($(BUILD_MSG))"

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -MMD -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

debug: 
	$(MAKE) BUILD_MODE=DEBUG all

release: 
	$(MAKE) BUILD_MODE=RELEASE all

debug-clean: clean
	$(MAKE) BUILD_MODE=DEBUG all

release-clean: clean
	$(MAKE) BUILD_MODE=RELEASE all

run: $(NAME)
	./$(NAME)

run-debug: debug
	./$(NAME)

gdb: debug
	gdb ./$(NAME)

valgrind: debug
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./$(NAME)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "Object files removed!"

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) deleted!"

re: 
	make fclean
	make all

build: 
	docker build -t ft_ping .
run_docker:
	docker run -it --rm --network=host --privileged ft_ping

-include $(DEPS)
