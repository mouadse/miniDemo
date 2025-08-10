SRC = main.c parsing/free.c parsing/tokenizer.c parsing/tokenizer_utils.c  parsing/glblist.c parsing/expanding_utils_extra.c\
	parsing/expanding.c parsing/print_helper.c parsing/expanding_utils.c parsing/check_errors.c parsing/tokenizer_for_expanding.c\
	builtins/cd.c builtins/echo.c builtins/env.c exec/global.c exec/redirection_utils.c exec/heredoc.c exec/execute_redirections.c exec/signals_handlers.c exec/signals_setup.c exec/exec_builtins.c builtins/exit.c builtins/exit_utils.c builtins/export.c builtins/pwd.c builtins/unset.c \
    env_var/env_init.c env_var/env_update.c env_var/env_utils.c env_var/env_utils1.c exec/args_utils.c  exec/exec_command.c exec/execute_pipes.c exec/execute_pipes2.c

NAME = minishell
HEADER = includes/minishell.h
OBJ = ${SRC:.c=.o}

CC = gcc 
CFLAGS = -Wall -Wextra -Werror -Iincludes -g3

LIB_DIR = libft
LIB_FILE = libft.a
LIB = $(LIB_DIR)/$(LIB_FILE)

RM = rm -f

all: ${NAME}

${NAME}: ${OBJ} ${HEADER} ${LIB}
	${CC} $(CFLAGS) ${OBJ} ${LIB} -lreadline -o ${NAME}

$(LIB):
	$(MAKE) -C $(LIB_DIR)

clean:
	${RM} ${OBJ}
	$(MAKE) clean -C $(LIB_DIR)
fclean:
	${RM} ${NAME} ${OBJ}
	$(MAKE) fclean -C $(LIB_DIR)
re: fclean all
