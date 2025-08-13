/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 23:00:52 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/13 13:50:13 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// TODO: value for param $?, probably from a struct ctx or something, also $? is a malloc value, not like getenv return
ssize_t	expand_param(t_seg *param, char **res)
{
	char	*key;

	*res = NULL;
	key = ft_strndup(param->slice.begin, param->slice.len);
	if (key == NULL)
		return (-1);
	*res = getenv(key);
	free(key);
	if (*res == NULL)
		*res = "";
	return (ft_strlen(*res));
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
	/* printf("Size: %zu, Wildcards: %d\n", len, wildcards);
	printf("Expanded token: %s\n", res); */
	return (res);
}

/* int	expand_cmd(t_cmd *cmd)
{

} */
