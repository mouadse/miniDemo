#include "minishell.h"

void	remove_quote(char *str, int start, int end)
{
	int	i;

	i = 0;
	while (str[start + i + 1] != 0)
	{
		str[start + i] = str[start + i + 1];
		i++;
	}
	start = end - 1;
	i = 0;
	while (str[start + i + 1] != 0)
	{
		str[start + i] = str[start + i + 1];
		i++;
	}
	str[ft_strlen(str) - 2] = 0;
}

int	quote_handling(t_tokenizer *token)
{
	int		i;
	char	q;
	int		start;
	int		j;

	i = 0;
	j = 0;
	while (token->str[i] != 0)
	{
		q = is_quote(token->str[i]);
		if (q != 0 && token->quotes_index != NULL && i == (token->quotes_index[j] - j))
		{
			start = i;
			i++;
			j++;
			while (q != is_quote(token->str[i])
				|| i != (token->quotes_index[j] - ((j -  1))))
				i++;
			remove_quote(token->str, start, i);
			j++;
			i -= 2;
		}
		i++;
	}
	return (0);
}

t_tokenizer	**env_var(t_tokenizer **token)
{
	int			i;
	char		c;
	int			to_dele;

	i = 0;
	while ((*token)->str[i] != 0)
	{
		c = (*token)->str[i];
		if (is_quote((*token)->str[i]))
		{
			i++;
			while ((*token)->str[i] != c)
				i++;
		}
		to_dele = expand_nq(token, &i);
		i++;
	}
	if (to_retokenize(token) == 1)
	{
		 	// printf("expand_nq : %s \n", (*token)->str);
			// exit(1);
		tokenize_the_envar(token);
	}
	if (!to_dele)
	{
		save_index(*token);
	}
	return (token);
}

void	expanding(t_tokenizer **token)
{
	t_tokenizer	**temp;

	temp = token;
	while ((*temp) != NULL)
	{
		// Do not read heredoc during expanding; it is handled at execution time
		if ((*temp)->op == LESS_LESS)
			(*temp)->hd = NULL;
		if ((*temp)->op == NOT_OP)
		{
			// printf("hada :%s\n",(*temp)->str);
			temp = env_var(temp);
    		// print_tokenizer(*token);
			// printf("\nhere\n");
		}
		if ((*temp)->op == NOT_OP)
			quote_handling((*temp));
		if ((*temp) == NULL)
			break ;
		temp = &(*temp)->next;
	}
}
