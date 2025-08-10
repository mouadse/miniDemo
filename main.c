#include "minishell.h"

t_env	*save_env(char **env)
{
	t_env	*env_list;
	t_env	*env_head;
	int		i;
	int		j;

	i = 0;
	env_head = malloc(sizeof(t_env));
	env_list = env_head;
	while (env[i] != 0)
	{
		j = 0;
		while (env[i][j] != '=')
			j++;
		env_list->name = ft_substr(env[i], 0, j);
		env_list->value = ft_substr(env[i], j + 1, ft_strlen(env[i]));
		i++;
		if (env[i] != NULL)
			env_list->next = malloc(sizeof(t_env));
		else
			break;
		env_list = env_list->next;
	}
	env_list->next = NULL;
	return (env_head);
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
		input = readline("\033[1;32m➜\033[0m\033[1;36m Minishell $> \033[0m");
		if (!input)
        {
            printf("exit\n");
			exit(1);
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
		set_signal_handler(tokens); //set+sognals
	//	print_tokenizer(tokens);
        if (has_pipe(tokens))
        {
            execute_pipeline(tokens);
            close_redirection_fds(tokens);
            free_tokens(input, tokens);
            continue;
        }
		redirection_infos(tokens);
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

					envp = envlist_to_array(glb_list()->env);
					if (!envp)
					{
						perror("envlist_to_array");
						free(path);
						exit(1);
					}

					execve(path, args, envp);
					perror("execve");
					free(path);
					free_strs(envp);
					exit(1);
				}
				else if (pid > 0)
                {
                    waitpid(pid, &exit_status, 0);
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
	return (glb_list()->exit_status);
}
