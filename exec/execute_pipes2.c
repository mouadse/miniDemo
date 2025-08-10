#include "minishell.h"

int count_pipes(t_tokenizer *tokens)
{
    int count = 0;
    t_tokenizer *temp;

    temp = tokens;
    while (temp)
    {
        if (temp->op == PIPE)
            count++;
        temp = temp->next;
    }
    return count;
}

t_tokenizer **split_into_commands(t_tokenizer *tokens, int *cmd_count)
{
    int pipe_count;
    t_tokenizer **commands;
    t_tokenizer *current;
    t_tokenizer *cmd_start;
    t_tokenizer *prev;
    int cmd_index;

    // Handle empty token list to avoid returning a non-null commands array with NULL entries
    if (!tokens)
    {
        *cmd_count = 0;
        return NULL;
    }

    pipe_count = count_pipes(tokens);
    *cmd_count = pipe_count + 1;

    commands = malloc(sizeof(t_tokenizer *) * (*cmd_count));
    if (!commands)
        return NULL;

    current = tokens;
    cmd_start = tokens;
    prev = NULL;
    cmd_index = 0;

    while (current)
    {
        if (current->op == PIPE)
        {
            if (prev)
                prev->next = NULL;

            commands[cmd_index++] = cmd_start;
            cmd_start = current->next;
        }
        prev = current;
        current = current->next;
    }
    commands[cmd_index] = cmd_start;

    return commands;
}

void execute_multiple_pipes(t_tokenizer *tokens)
{
    int cmd_count;
    t_tokenizer **commands;

    commands = split_into_commands(tokens, &cmd_count);
    if (!commands || cmd_count == 0)
        return;

    int pipes_count = cmd_count - 1;

    // Allocate pipes dynamically only when needed (avoid zero-length VLA)
    int (*pipes)[2] = NULL;
    if (pipes_count > 0)
    {
        pipes = malloc(sizeof(int[2]) * pipes_count);
        if (!pipes)
        {
            free(commands);
            return;
        }
    }

    pid_t *pids = malloc(sizeof(pid_t) * cmd_count);
    if (!pids)
    {
        free(commands);
        if (pipes)
            free(pipes);
        return;
    }
    // Initialize to a known value to avoid reading garbage later
    for (int i = 0; i < cmd_count; i++)
        pids[i] = -1;

    for (int i = 0; i < pipes_count; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("minishell: pipe");
            // Close any previously opened pipe fds
            for (int j = 0; j < i; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            free(commands);
            free(pids);
            if (pipes)
                free(pipes);
            return;
        }
    }

    for (int i = 0; i < cmd_count; i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            if (i > 0 && pipes_count > 0)
            {
                if (dup2(pipes[i-1][0], STDIN_FILENO) < 0)
                {
                    perror("minishell: dup2");
                    exit(1);
                }
            }
            if (i < cmd_count - 1 && pipes_count > 0)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0)
                {
                    perror("minishell: dup2");
                    exit(1);
                }
            }
            for (int j = 0; j < pipes_count; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // No command tokens for this slot (shouldn't happen after early check, but guard anyway)
            if (!commands[i])
                exit(0);

            if (execute_redirections(commands[i]))
                exit(1);

            char **args = tokens_to_args(commands[i]);
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

            char *path = get_cmd_path(args[0], glb_list()->env);
            if (!path)
            {
                ft_putstr_fd("minishell: command not found: ", 2);
                ft_putstr_fd(args[0], 2);
                ft_putchar_fd('\n', 2);
                free_args(args);
                exit(127);
            }

            char **envp = envlist_to_array(glb_list()->env);
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
        {
            pids[i] = pid;
            if (i < 2)
                glb_list()->pid_pipeline[i] = pid;
        }
        else
        {
            perror("fork");
            // pids[i] is already -1 from initialization
            break;
        }
    }

    for (int i = 0; i < pipes_count; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    signal(SIGINT, signal_pipeline);
    signal(SIGQUIT, signal_pipeline);

    int status;
    for (int i = 0; i < cmd_count; i++)
    {
        if (pids[i] > 0)
        {
            waitpid(pids[i], &status, 0);
            if (i == cmd_count - 1)
            {
                if (WIFEXITED(status))
                    glb_list()->exit_status = WEXITSTATUS(status);
                else if (WIFSIGNALED(status))
                    glb_list()->exit_status = 128 + WTERMSIG(status);
            }
        }
    }

    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    glb_list()->pid_pipeline[0] = 0;
    glb_list()->pid_pipeline[1] = 0;

    free(commands);
    free(pids);
    if (pipes)
        free(pipes);
}
