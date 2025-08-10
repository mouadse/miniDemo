#include "minishell.h"

char	*check_env(char *str)
{
	t_env	*env;

	env = glb_list()->env;
	while (env != 0)
	{
		if (ft_strncmp(str, env->name, ft_strlen(env->name) + 1) == 0)
		{
			free(str);
			return (env->value);
		}
		env = env->next;
	}
	free(str);
	return (NULL);
}

char	*new_alloc(char *str, char *env_value, int *start, int len)
{
	int		new_len;
	char	*new_str;
	int		new_start;

	new_len = ft_strlen(str) - len + ft_strlen(env_value);
	new_str = malloc(new_len + 1);
	ft_strlcpy(new_str, str, *start + 1);
	ft_strlcat(new_str, env_value, new_len + 1);
	new_start = ft_strlen(new_str);
	ft_strlcat(new_str, str + *start + len, new_len + 1);
	*start = new_start - 1;
	return (new_str);
}

char	*re_alloc(char *str, int *start, int len, char *env_value)
{
	char	*new_str;

	if (env_value == NULL)
	{
		ft_memmove(str + *start, str + *start + len, ft_strlen(str + *start + len) + 1);
		*start -= 1;
		return (str);
	}
	new_str = new_alloc(str, env_value, start, len);
	free(str);
	return (new_str);
}

t_here_doc	*here_doc(t_tokenizer *token)
{
	char		*delemeter;
	t_here_doc	*hd;
	t_here_doc	*head;

	hd = malloc(sizeof(t_here_doc));
	head = hd;
	hd->str = token->next->str;
	delemeter = hd->str;
	hd->next = malloc(sizeof(t_here_doc));
	hd = hd->next;
	hd->str = readline("> ");
	while (ft_strncmp(delemeter, hd->str, ft_strlen(hd->str)) != 0)
	{
		hd->next = malloc(sizeof(t_here_doc));
		hd = hd->next;
		hd->str = readline("> ");
	}
	hd->next = NULL;
	return (head);
}

void	tokenize_the_envar(t_tokenizer **token)
{
	t_tokenizer	**token_temp;
	t_tokenizer	*token_next;
	t_tokenizer	*token_dele;
	char		*str;

	
	token_dele = *token;
	str = (*token)->str;
	token_next = (*token)->next;
	*token = tokenizer_for_expanding((*token)->str);
	printf("this is pointer token : %p and %ld and %s\n",*token ,malloc_usable_size(*token), (*token)->str);
	printf("this is second point :%p and %ld and %s\n",token_dele, malloc_usable_size(token_dele), (token_dele)->str);
	free(str);
	free(token_dele);
	token_temp = token;
	printf("this is second point :%p and %ld and %s\n",token_temp, malloc_usable_size(*token_temp), (*token_temp)->str);
	while ((*token_temp) != NULL)
	{
		token_temp = &(*token_temp)->next;
	}
	*token_temp = token_next;
}
