/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsennane <hsennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 04:45:49 by hsennane          #+#    #+#             */
/*   Updated: 2025/08/13 04:45:52 by hsennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_execve_error(char **args, char *path, char **envp)
{
	int	saved_errno;

	saved_errno = errno;
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(args[0], 2);
	ft_putstr_fd(": ", 2);
	ft_putendl_fd(strerror(saved_errno), 2);
	if (path)
		free(path);
	if (envp)
		free_strs(envp);
	if (saved_errno == ENOENT)
		exit(127);
	else if (saved_errno == EACCES || saved_errno == ENOTDIR
		|| saved_errno == EISDIR || saved_errno == ENOEXEC)
		exit(126);
	else
		exit(126);
}

void	perform_execve(char **args, char *path, t_env *env)
{
	char	**envp;

	envp = envlist_to_array(env);
	if (!envp)
	{
		ft_putendl_fd("minishell: envlist_to_array failed", 2);
		free(path);
		exit(1);
	}
	execve(path, args, envp);
	handle_execve_error(args, path, envp);
}

int	has_pipe(t_tokenizer *tokens)
{
	while (tokens)
	{
		if (tokens->op == PIPE)
			return (1);
		tokens = tokens->next;
	}
	return (0);
}

void	ignore_interactive_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
