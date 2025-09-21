NAME = minishell

CC ?= cc
RM = rm -f

INC_DIR = include
LIBFT_DIR = libft

CFLAGS = -Wall -Wextra -Werror $(INCFLAGS) $(COPT)
COPT ?= -O2
INCFLAGS = -I$(INC_DIR) -I$(LIBFT_DIR)
LDLIBS = -lreadline

LIBFT = $(LIBFT_DIR)/libft.a

SRC_DIR = source
SOURCES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/repl.c \
	$(SRC_DIR)/env.c \
	$(SRC_DIR)/init.c \
	$(SRC_DIR)/signals.c \
	$(SRC_DIR)/errors.c \
	$(SRC_DIR)/errors2.c \
	$(SRC_DIR)/lexer/free.c \
	$(SRC_DIR)/lexer/general.c \
	$(SRC_DIR)/lexer/handlers.c \
	$(SRC_DIR)/lexer/lexer.c \
	$(SRC_DIR)/lexer/segments.c \
	$(SRC_DIR)/lexer/tokens.c  \
	$(SRC_DIR)/parser/command.c \
	$(SRC_DIR)/parser/free.c \
	$(SRC_DIR)/parser/heredoc.c \
	$(SRC_DIR)/parser/heredoc_write.c \
	$(SRC_DIR)/parser/operators.c \
	$(SRC_DIR)/parser/parser.c \
	$(SRC_DIR)/parser/redir.c \
	$(SRC_DIR)/parser/redirs.c \
	$(SRC_DIR)/expansion/alternatives.c \
	$(SRC_DIR)/expansion/atoms.c \
	$(SRC_DIR)/expansion/command.c \
	$(SRC_DIR)/expansion/expansion.c \
	$(SRC_DIR)/expansion/free.c \
	$(SRC_DIR)/expansion/free2.c \
	$(SRC_DIR)/expansion/param.c \
	$(SRC_DIR)/expansion/redirs.c \
	$(SRC_DIR)/expansion/structures.c \
	$(SRC_DIR)/expansion/wildcards.c \
	$(SRC_DIR)/expansion/wildcards2.c \
	$(SRC_DIR)/execute/builtin_exec.c \
	$(SRC_DIR)/execute/cmd_exec.c \
	$(SRC_DIR)/execute/cmd_path.c \
	$(SRC_DIR)/execute/execute.c \
	$(SRC_DIR)/execute/pipe_exec.c \
	$(SRC_DIR)/builtins/pwd.c \
	$(SRC_DIR)/builtins/env.c \
	$(SRC_DIR)/builtins/unset.c \
	$(SRC_DIR)/builtins/echo.c \
	$(SRC_DIR)/builtins/exit.c \
	$(SRC_DIR)/builtins/export.c \
	$(SRC_DIR)/builtins/export2.c \
	$(SRC_DIR)/debug.c
BONUS_SOURCES = $(SOURCES)

OBJ = $(SOURCES:%.c=%.o)
BONUS_OBJ = $(BONUS_SOURCES:%.c=%.o)

NORMAL_FLAG = .all_build
BONUS_FLAG = .bonus_build

all: $(NORMAL_FLAG)

bonus: $(BONUS_FLAG)

$(NAME): $(LIBFT) $(OBJ)
	@printf '$(DIM)Linking $(BOLD)$(PRIMARY)$(NAME)$(RST)$(DIM)...\n$(RST)'
	@$(CC) $(OBJ) $(CFLAGS) $(LIBFT) $(LDLIBS) -o $(NAME)

$(NORMAL_FLAG): $(NAME)
	@printf '$(BOLD)$(PRIMARY)$(BANNER)$(RST)$(BOLD)  Mini Yeska  \n\n$(RST)'
	@$(RM) $(BONUS_FLAG)
	@touch $@

$(BONUS_FLAG): $(NAME)
	@printf '$(BOLD)$(PRIMARY)$(BANNER)$(RST)$(BOLD)  Mini Yeska  \n\n$(RST)'
	@$(RM) $(NORMAL_FLAG)
	@touch $@

$(LIBFT):
	@printf '$(DIM)Building $(WHITE)libft$(DIM)...\n$(RST)'
	@make -C $(LIBFT_DIR) --no-print-directory

%.o: %.c
	@printf '$(DIM)Compiling $(WHITE)$<$(DIM)...\n$(RST)'
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@printf '$(DIM)Cleaning…$(RST)\n'
	@make -C $(LIBFT_DIR) fclean --no-print-directory
	@$(RM) $(OBJ) $(BONUS_OBJ)
	@$(RM) $(BONUS_FLAG) $(NORMAL_FLAG)

fclean: clean
	@printf '$(DIM)$(RM) $(BOLD)$(PRIMARY)$(NAME)$(RST)\n'
	@$(RM) $(NAME)

re: fclean all

debug: CFLAGS += -g3 -fno-omit-frame-pointer
debug: COPT = -Og
debug: all

dev: CFLAGS += -g3 -fno-omit-frame-pointer -fsanitize=address,leak,undefined
dev: COPT = -Og
dev: all

.PHONY: all bonus clean fclean re debug dev

BANNER = \n       , \
\n       |\--._ \
\n       /   _/ \
\n      /`  } \
\n     (  \ / \
\n \ /`   \\\\\ \
\n  `\    /_\\\ \
\n   `~~~~~``~` \
\n

PRIMARY = \033[31m
BOLD = \033[1m
DIM = \033[2m
WHITE = \033[37m
RST = \033[0m
