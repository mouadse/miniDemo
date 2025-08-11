/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mouad <mouad@student.42.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 00:13:00 by mouad             #+#    #+#             */
/*   Updated: 2025/08/12 00:13:00 by mouad            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	open_redir_fd(t_tokenizer *op_tok)
{
	int			fd;
	t_env		*env;

	fd = -1;
	env = glb_list()->env;
	if (!op_tok || !op_tok->next)
		return (-1);
	if (op_tok->op == GREAT)
		fd = open(op_tok->next->str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (op_tok->op == GREAT_GREAT)
		fd = open(op_tok->next->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (op_tok->op == LESS)
		fd = open(op_tok->next->str, O_RDONLY);
	else if (op_tok->op == LESS_LESS)
	{
		if (op_tok->next->redirect.file_fd >= 0)
			fd = op_tok->next->redirect.file_fd;
		else
			fd = open_heredoc_and_write_pipe(op_tok->next, env, NULL);
	}
	return (fd);
}

static int	setup_out_redir(t_tokenizer *tok)
{
	int	fd;

	fd = tok->next->redirect.file_fd;
	if (fd < 0)
		fd = open_redir_fd(tok);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(tok->next->str, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putchar_fd('\n', 2);
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		if (fd > 2)
			close(fd);
		return (1);
	}
	if (fd > 2)
		close(fd);
	return (0);
}

static int	setup_in_redir(t_tokenizer *tok)
{
	int	fd;

	fd = tok->next->redirect.file_fd;
	if (fd < 0)
		fd = open_redir_fd(tok);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(tok->next->str, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putchar_fd('\n', 2);
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("dup2");
		if (fd > 2)
			close(fd);
		return (1);
	}
	if (fd > 2)
		close(fd);
	return (0);
}

int	execute_redirections(t_tokenizer *tokens)
{
	while (tokens)
	{
		if ((tokens->op == GREAT || tokens->op == GREAT_GREAT)
			&& tokens->next)
		{
			if (setup_out_redir(tokens))
				return (1);
		}
		else if ((tokens->op == LESS || tokens->op == LESS_LESS)
			&& tokens->next)
		{
			if (setup_in_redir(tokens))
				return (1);
		}
		tokens = tokens->next;
	}
	return (0);
}
