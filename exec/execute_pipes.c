#include "minishell.h"

t_tokenizer *split_tokens_at_pipe(t_tokenizer *tokens, t_tokenizer **right_side)
{
    t_tokenizer *temp;
    t_tokenizer *prev;

    temp = tokens;
    prev = NULL;
    while (temp && temp->op != PIPE)
    {
        prev = temp;
        temp = temp->next;
    }
    if (!temp)
    {
        *right_side = NULL;
        return (tokens);
    }
    if (prev)
        prev->next = NULL;
    *right_side = temp->next;
    temp->next = NULL;

    return (tokens);
}

void     signal_pipeline(int sig)
{
    if (sig == SIGINT)
    {
        if (glb_list()->pid_pipeline[0] > 0)
            kill(glb_list()->pid_pipeline[0], SIGINT);
        if (glb_list()->pid_pipeline[1] > 0)
            kill(glb_list()->pid_pipeline[1], SIGINT);
    }
    else if (sig == SIGQUIT)
    {
        if (glb_list()->pid_pipeline[0] > 0)
            kill(glb_list()->pid_pipeline[0], SIGQUIT);
        if (glb_list()->pid_pipeline[1] > 0)
            kill(glb_list()->pid_pipeline[1], SIGQUIT);
    }
}

int     has_pipe(t_tokenizer *tokens)
{
    t_tokenizer *temp = tokens;

    while (temp)
    {
        if (temp->op == PIPE)
            return (1);
        temp = temp->next;
    }
    return (0);
}
static void execute_pipeline_left(t_tokenizer *left_tokens, int *pipe_end)
{
    char **args;
    char *path;
    char **envp;
    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        if (dup2(pipe_end[1], STDOUT_FILENO) < 0)
        {
            perror("minishell: dup2");
            exit(1);
        }
        close(pipe_end[0]);
        close(pipe_end[1]);
        if (execute_redirections(left_tokens))
            exit(1);
        args = tokens_to_args(left_tokens);
        if (!args || !args[0] || args[0][0] == 0)
        {
            if (args)
                free_args(args);
            exit(0);
        }
        if (execute_builtin(args, &glb_list()->env, &glb_list()->exit_status) == 1)
        {
            free_args(args);
            exit(glb_list()->exit_status);
        }
        path = get_cmd_path(args[0], glb_list()->env);
        if (!path)
        {
            ft_putstr_fd("minishell: command not found: ", 2);
            ft_putstr_fd(args[0], 2);
            ft_putchar_fd('\n', 2);
            free_args(args);
            exit(127);
        }
        envp = envlist_to_array(glb_list()->env);
        if (!envp)
        {
            perror("envlist_to_array");
            free(path);
            free_args(args);
            exit(1);
        }
        execve(path, args, envp);
        perror("execve");
        free(path);
        free_strs(envp);
        free_args(args);
        exit(1);
    }
    else if (pid > 0)
        glb_list()->pid_pipeline[0] = pid;
    else
        perror("fork");
}

static void execute_pipeline_right(t_tokenizer *right_tokens, int *pipe_end)
{
    char **args;
    char *path;
    char **envp;
    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        if (dup2(pipe_end[0], STDIN_FILENO) < 0)
        {
            perror("minishell: dup2");
            exit(1);
        }
        close(pipe_end[0]);
        close(pipe_end[1]);
        if (execute_redirections(right_tokens))
            exit(1);
        args = tokens_to_args(right_tokens);
        if (!args || !args[0] || args[0][0] == 0)
        {
            if (args)
                free_args(args);
            exit(0);
        }
        if (execute_builtin(args, &glb_list()->env, &glb_list()->exit_status) == 1)
        {
            free_args(args);
            exit(glb_list()->exit_status);
        }
        path = get_cmd_path(args[0], glb_list()->env);
        if (!path)
        {
            ft_putstr_fd("minishell: command not found: ", 2);
            ft_putstr_fd(args[0], 2);
            ft_putchar_fd('\n', 2);
            free_args(args);
            exit(127);
        }
        envp = envlist_to_array(glb_list()->env);
        if (!envp)
        {
            perror("envlist_to_array");
            free(path);
            free_args(args);
            exit(1);
        }
        execve(path, args, envp);
        perror("execve");
        free(path);
        free_strs(envp);
        free_args(args);
        exit(1);
    }
    else if (pid > 0)
        glb_list()->pid_pipeline[1] = pid;
    else
        perror("fork");
}

void execute_pipeline(t_tokenizer *tokens)
{
    int pipe_count = count_pipes(tokens);

    if (pipe_count == 0)
        return;
    else if (pipe_count == 1)
    {
        int pipe_end[2];
        t_tokenizer *left_tokens;
        t_tokenizer *right_tokens;
        int status;

        glb_list()->pid_pipeline[0] = 0;
        glb_list()->pid_pipeline[1] = 0;

        left_tokens = split_tokens_at_pipe(tokens, &right_tokens);
        if (!right_tokens)
            return;

        if (pipe(pipe_end) < 0)
        {
            perror("minishell: pipe");
            return;
        }

        execute_pipeline_left(left_tokens, pipe_end);
        execute_pipeline_right(right_tokens, pipe_end);

        close(pipe_end[0]);
        close(pipe_end[1]);

        signal(SIGINT, signal_pipeline);
        signal(SIGQUIT, signal_pipeline);

        if (glb_list()->pid_pipeline[0] > 0)
        {
            waitpid(glb_list()->pid_pipeline[0], &status, 0);
            if (WIFEXITED(status))
                glb_list()->exit_status = WEXITSTATUS(status);
        }
        if (glb_list()->pid_pipeline[1] > 0)
        {
            waitpid(glb_list()->pid_pipeline[1], &status, 0);
            if (WIFEXITED(status))
                glb_list()->exit_status = WEXITSTATUS(status);
        }

        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        glb_list()->pid_pipeline[0] = 0;
        glb_list()->pid_pipeline[1] = 0;
    }
    else
    {
        execute_multiple_pipes(tokens);
    }
}
