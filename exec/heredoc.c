#include "minishell.h"


int open_heredoc_and_write_pipe(t_tokenizer *token, t_env *env, int *exit_status)
{
    int     pipefd[2];
    char    *line;

    (void)env;
	(void)exit_status; // If exit_status is not used, you can remove this line
    // *exit_status = 0;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        // *exit_status = 1;
        return -1;
    }

    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            ft_putstr_fd("minishell: warning: here-document delimited by end-of-file (wanted `", 2);
            ft_putstr_fd(token->str, 2);
            ft_putstr_fd("')\n", 2);
            break;
        }
        if (ft_strcmp(line, token->str) == 0)
        {
            free(line);
            break;
        }
        write(pipefd[1], line, ft_strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }

    close(pipefd[1]);
    return pipefd[0];
}


void process_heredocs(t_tokenizer *tokens, t_env *env, int *exit_status) {
    while (tokens) {
        if (tokens->op == LESS_LESS) {
            tokens = tokens->next;
            if (tokens)
                tokens->redirect.file_fd = open_heredoc_and_write_pipe(tokens, env, exit_status);
        }
        if (tokens)
            tokens = tokens->next;
    }
}
