/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 23:00:52 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/15 15:12:58 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct s_param
{
	size_t			len;
	char			*value;
	struct s_param	*next;
}	t_param;

typedef struct s_params
{
	t_param	*head;
	t_param	*tail;
}	t_params;

typedef struct s_exp
{
	t_params	params;
	size_t		len;
	size_t		wildcards;
}	t_exp;

typedef struct s_argv
{
	t_list	*head;
	t_list	*tail;
	size_t	argc;
}	t_argv;

// TODO: in all functions, must use own getenv function because when i'm using getenv, it will not be updated for the current process, so probably can forget the use of some frees
//
/*
	? Idea for structs of env:
		typedef struct s_env
		{
			char	*key;
			char	*value;
			struct s_env *next;
		} t_env;

		typedef struct s_env_list
		{
			char	**envp; // On unset/expand must set to NULL (free and null set), on execvp must be set
			t_env *head;
			t_env *tail;
			size_t	length;
		} t_env_list;
*/

t_param	*expand_param(t_seg *seg, int status)
{
	char	*key;
	t_param	*res;

	res = malloc(sizeof(t_param));
	if (res == NULL)
		return (NULL);
	if (seg->slice.len == 1 && seg->slice.begin[0] == '?')
	{
		res->value = ft_itoa(status);
		res->len = ft_strlen(res->value);
		return (res);
	}
	key = ft_strndup(seg->slice.begin, seg->slice.len);
	if (key == NULL)
		return (free(res), NULL);
	res->value = getenv(key);
	free(key);
	if (res->value == NULL)
		res->value = "";
	res->len = ft_strlen(res->value);
	return (res);
}

void	param_push(t_params *params, t_param *new_param)
{
	if (params->head == NULL)
		params->head = new_param;
	else
		params->tail->next = new_param;
	params->tail = new_param;
}

static int	get_info(t_exp *exp, t_tok *tok)
{
	t_seg	*cur;
	t_param	*param;

	cur = tok->seg_head;
	while (cur != NULL)
	{
		if (cur->type == SEG_TEXT)
			exp->len += cur->slice.len;
		else if (cur->type == SEG_PARAM)
		{
			param = expand_param(cur, 255); // TODO: use real last status code instead of 0
			if (param == NULL)
				return (perror_malloc(), 1);
			param_push(&exp->params, param);
			exp->len += param->len;
		}
		else if (cur->type == SEG_WILDCARD)
		{
			exp->wildcards++;
			exp->len++;
		}
		cur = cur->next;
	}
	return (0);
}

static int	expand_wildcard(t_exp *exp, t_tok *tok, t_argv *argv)
{
	// TODO: handle wildcards, can push more than 1 value
	(void)exp;
	(void)tok;
	(void)argv;
	return (0);
}

static size_t	consume_param(t_exp *exp, t_seg *seg, char *cur)
{
	t_param	*tmp;
	size_t	len;

	tmp = exp->params.head;
	len = tmp->len;
	ft_strlcpy(cur, tmp->value, tmp->len + 1);
	exp->params.head = exp->params.head->next;
	if (seg->slice.len == 1 && seg->slice.begin[0] == '?')
		free(tmp->value);
	free(tmp);
	if (exp->params.head == NULL)
		exp->params.tail = NULL;
	return (len);
}

static int	argv_push(t_argv *argv, char *value)
{
	t_list	*new_node;

	new_node = ft_lstnew(value);
	if (new_node == NULL)
		return (1);
	if (argv->head == NULL)
		argv->head = new_node;
	else
		argv->tail->next = new_node;
	argv->tail = new_node;
	argv->argc++;
	return (0);
}

static int	expand_word(t_exp *exp, t_tok *tok, t_argv *argv)
{
	t_seg	*cur;
	size_t	i;
	char	*expanded;

	expanded = malloc(sizeof(char ) * (exp->len + 1));
	if (expanded == NULL)
		return (1);
	i = 0;
	cur = tok->seg_head;
	while (cur != NULL)
	{
		if (cur->type == SEG_TEXT)
		{
			ft_strlcpy(&expanded[i], cur->slice.begin, cur->slice.len + 1);
			i += cur->slice.len;
		}
		else if (cur->type == SEG_PARAM)
			i += consume_param(exp, cur, &expanded[i]);
		cur = cur->next;
	}
	if (argv_push(argv, expanded) == 1)
		return (free(expanded), 1);
	return (0);
}

int	expansion(t_tok *tok, t_argv *argv)
{
	t_exp	exp;

	exp.params.head = NULL;
	exp.params.tail = NULL;
	exp.len = 0;
	exp.wildcards = 0;
	if (get_info(&exp, tok) == 1)
		return (1); // TODO: free params list on error
	if (exp.wildcards > 0)
	{
		if (expand_wildcard(&exp, tok, argv) == 1)
			return (1); // TODO: free params list on error
	}
	else
	{
		if (expand_word(&exp, tok, argv) == 1)
			return (1); // TODO: free params list on error
	}
	return (0);
}

int	redir_expansion(t_redir *redir)
{
	t_argv	argv;

	argv.head = NULL;
	argv.tail = NULL;
	argv.argc = 0;
	if (expansion(redir->u_data.word, &argv) == 1)
		return (1);
	if (argv.argc > 1)
		return (perror_ambiguosredir(redir->u_data.word),
			free_tok(&redir->u_data.word), ft_lstclear(&argv.head, free), 1);
	free_tok(&redir->u_data.word);
	redir->u_data.name = (char *)argv.head->content;
	ft_lstdelone(argv.head, NULL);
	return (0);
}

int	redirs_expansion(t_redirs *redirs)
{
	t_redir	*cur;

	cur = redirs->head;
	while (cur != NULL)
	{
		if (cur->type != R_HEREDOC)
		{
			if (redir_expansion(cur) == 1)
			{
				// TODO: handle error, prev nodes must be free knowing are char *, remaining nodes must be freed as t_tok *
				return (1);
			}
		}
		cur = cur->next;
	}
	return (0);
}

int	expand_cmd(t_cmd *cmd) // also add $?
{
	t_argv	argv;
	t_tok	*cur;
	size_t		i;
	t_list	*next;

	if (redirs_expansion(&cmd->redir) == 1)
		return (1);
	/*
		expand each member of cmd->u_data.words, next free cmd->u_data.words
		and save the values of argv in cmd->u_data.argv, from t_list to char **
		with the size of argv.argc
	*/
	argv.head = NULL;
	argv.tail = NULL;
	argv.argc = 0;
	cur = cmd->u_data.words;
	while (cur != NULL)
	{
		if (expansion(cur, &argv) == 1)
			return (1); // TODO: free remaining t_tok, and created argv
		cur = cur->next;
	}
	cmd->u_data.argv = malloc(sizeof(char *) * (argv.argc + 1));
	if (cmd->u_data.argv == NULL)
		return (1); // TODO: free created argv, and redirs using the name (char *)
	cmd->count = argv.argc;
	i = 0;
	while (i < argv.argc)
	{
		cmd->u_data.argv[i] = argv.head->content;
		next = argv.head->next;
		ft_lstdelone(argv.head, NULL);
		argv.head = next;
		i++;
	}
	return (0);
}

/*
int	expand_subsh(t_subsh *subsh) // also add $?
{
	if (redirs_expansion(&subsh->redir) == 1)
		return (1);
	return (0);
}
*/

/*
typedef struct s_param
{
	size_t			len;
	char			*value;
	struct s_param	*next;
}	t_param;

t_param	*expand_param(t_seg *param)
{
	char	*key;
	t_param	*res;

	res = malloc(sizeof(t_param));
	if (res == NULL)
		return (NULL);
	key = ft_strndup(param->slice.begin, param->slice.len);
	if (key == NULL)
		return (free(res), NULL);
	res->value = getenv(key);
	free(key);
	if (res->value == NULL)
		res->value = "";
	res->len = ft_strlen(res->value);
	return (res);
}

// TODO: instead of use t_list, think about use a dynamic array, also dynamic array can be used with the wildcard
// TODO: or create an specific t_list for char * with the lenght, also consider return a t_list too for the wildcard return (TODO)
char	*expand_tok(t_tok *tok)
{
	char	*res;
	t_seg	*cur;
	ssize_t	param_len;
	t_list	*param_expanded;
	t_list	*next_param;
	char		*param;
	size_t	len;
	int		wildcards;
	size_t	i;

	param_expanded = NULL;
	cur = tok->seg_head;
	wildcards = 0;
	len = 0;
	while (cur != NULL)
	{
		if (cur->type == SEG_WILDCARD)
		{
			wildcards++;
			len++;
		}
		else if (cur->type == SEG_TEXT)
			len += cur->slice.len;
		else if (cur->type == SEG_PARAM)
		{
			param_len = expand_param(cur, &param);
			if (param_len == -1)
				return (ft_lstclear(&param_expanded, NULL), NULL);
			next_param = ft_lstnew(param);
			if (next_param == NULL)
				return (ft_lstclear(&param_expanded, NULL), NULL);
			ft_lstadd_back(&param_expanded, next_param);
			len += param_len;
		}
		cur = cur->next;
	}
	// if (wildcards > 0)
		// return (NULL); // TODO: manage wildcards for return char ** when do globbing, not char *, if don't find matches with *, must return the char* consturcted by concat * with the expanded info
	res = malloc(sizeof(char) * (len + 1));
	if (res == NULL)
		return (ft_lstclear(&param_expanded, NULL), NULL);
	cur = tok->seg_head;
	i = 0;
	while (cur != NULL)
	{
		if (cur->type == SEG_WILDCARD)
			res[i++] = '*';
		else if (cur->type == SEG_TEXT)
		{
			ft_strlcpy(&res[i], cur->slice.begin, cur->slice.len + 1);
			i += cur->slice.len;
		}
		else if (cur->type == SEG_PARAM)
		{
			param_len = ft_strlen(param_expanded->content);
			ft_strlcpy(&res[i], param_expanded->content, param_len + 1);
			i += param_len;
			// consume t_list node
			t_list *temp = param_expanded;
			param_expanded = param_expanded->next;
			ft_lstdelone(temp, NULL);
		}
		cur = cur->next;
	}
	res[i] = '\0';
	printf("Size: %zu, Wildcards: %d\n", len, wildcards);
	printf("Expanded token: %s\n", res);
	return (res);
}

int	expand_cmd(t_cmd *cmd)
{

} */
