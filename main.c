#include "minishell.h"


t_env *save_env(char **env)
{
    t_env *head = NULL;
    t_env *tail = NULL;
    int i = 0;
    int j;

    if (!env)
        return NULL;
    while (env[i] != 0)
    {
        t_env *new_node = gc_alloc(sizeof(t_env));
        if (!new_node)
            return NULL;

        j = 0;
        while (env[i][j] && env[i][j] != '=')
            j++;

        if (env[i][j] == '=')
        {
            new_node->name = ft_substr(env[i], 0, j);
            new_node->value = ft_substr(env[i], j + 1, ft_strlen(env[i]));
        }
        else
        {
            new_node->name = ft_strdup(env[i]);
            new_node->value = ft_strdup("");
        }
        new_node->next = NULL;

        if (head == NULL)
        {
            head = new_node;
            tail = new_node;
        }
        else
        {
            tail->next = new_node;
            tail = new_node;
        }
        i++;
    }
    return head;
}

int main(int ac, char **av, char **env)
{

    t_tokenizer *tokens;
    char *input;
    char **args;
    int	exit_status;
    char **envp;
	char *path;

  (void)ac;
  (void)av;

//   glb_list()->env = NULL;
//   init_env_var(&glb_list()->env, env);
	setup_signals();
	glb_list()->env = save_env(env);
	glb_list()->exit_status = 0;
    while (1)
    {
        /* Restore interactive signal behavior before reading a new line */
        setup_signals();
		input = readline("\033[1;32m➜\033[0m\033[1;36m Minishell $> \033[0m");
        if (!input)
        {
            printf("exit\n");
            free_env(glb_list()->env);
            rl_clear_history();
            /* Exit with last command's status instead of hardcoded 1 */
			exit(glb_list()->exit_status);
        }
		if (ft_strlen(input) == 0)
        {
            free(input);
            continue;
        }
		if (input_error(input) == 1)
		{
			free(input);
			continue;
		}

		add_history(input);
		tokens = tokenizer(input);

		if (check_parsing_errors(tokens))
		{
			free_tokens(input, tokens);
			continue;
		}

		expanding(&tokens);
		set_signal_handler(tokens); // only specializes for heredoc
		init_redirect_fds(tokens);
		redirection_infos(tokens);
	//	print_tokenizer(tokens);
		if (has_pipe(tokens))
		{
			execute_pipeline(tokens, glb_list(), &exit_status);
			close_redirection_fds(tokens);
			free_tokens(input, tokens);
			continue;
		}
		args = tokens_to_args(tokens);
		if (args && args[0] && args[0][0] != 0)
		{
			// hna kadar main execution
			exit_status = 0;

			//special cases li khlat builtins maykhdmouch f akher version.
			if (is_builtin(args[0]) && (ft_strcmp(args[0], "cd") == 0 ||
				ft_strcmp(args[0], "export") == 0 ||
				ft_strcmp(args[0], "unset") == 0 ||
				ft_strcmp(args[0], "exit") == 0))
			{
				execute_builtin(args, &glb_list()->env, &exit_status);
				glb_list()->exit_status = exit_status;
			}
			else
			{
				pid_t pid = fork();
				if (pid == 0)
				{
                        /* In child: default signals so external cmds see SIGINT/SIGQUIT */
                        struct sigaction act;
                        sigemptyset(&act.sa_mask);
                        act.sa_flags = 0;
                        act.sa_handler = SIG_DFL;
                        sigaction(SIGINT, &act, NULL);
                        sigaction(SIGQUIT, &act, NULL);
					if (execute_redirections(tokens))
						exit(1);

					if (execute_builtin(args, &glb_list()->env, &exit_status) == 1)
						exit(exit_status);

					path = get_cmd_path(args[0], glb_list()->env);
					if (!path)
					{
						ft_putstr_fd("minishell: command not found: ", 2);
						ft_putstr_fd(args[0], 2);
						ft_putchar_fd('\n', 2);
						glb_list()->exit_status = 127;
						exit(127);
					}

					/* Handle directories specifically */
					struct stat path_stat;
					if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
					{
						ft_putstr_fd("minishell: ", 2);
						ft_putstr_fd(args[0], 2);
						ft_putstr_fd(": Is a directory\n", 2);
						free(path);
						exit(126); /* 126 is the correct exit code for permission denied */
					}

					envp = envlist_to_array(glb_list()->env);
					if (!envp)
					{
						perror("envlist_to_array");
						free(path);
						exit(1);
					}

					execve(path, args, envp);
					{
						int saved_errno = errno;
						ft_putstr_fd("minishell: ", 2);
						ft_putstr_fd(args[0], 2);
						ft_putstr_fd(": ", 2);
						ft_putendl_fd(strerror(saved_errno), 2);
						free(path);
						free_strs(envp);
						if (saved_errno == ENOENT)
							exit(127);
						else if (saved_errno == EACCES || saved_errno == ENOTDIR || saved_errno == EISDIR || saved_errno == ENOEXEC)
							exit(126);
						else
							exit(126);
					}
				}
				else if (pid > 0)
				{
					/* Parent: ignore interactive signals while waiting */
					signal(SIGINT, SIG_IGN);
					signal(SIGQUIT, SIG_IGN);
					waitpid(pid, &exit_status, 0);
					/* Restore interactive handlers for next prompt */
					setup_signals();
					if (WIFEXITED(exit_status))
						glb_list()->exit_status = WEXITSTATUS(exit_status);
					else if (WIFSIGNALED(exit_status))
						glb_list()->exit_status = 128 + WTERMSIG(exit_status);
				}
                else
                {
                    perror("fork");
                    glb_list()->exit_status = 1;
                }
			}
		}
		close_redirection_fds(tokens);
		free_args(args);
		free_tokens(input, tokens);
	}
	gc_free_all();
	return (glb_list()->exit_status);
}
