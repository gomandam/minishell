/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   param.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 20:28:03 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/30 14:57:04 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
t_param	*expand_param(t_shell *shell, t_seg *seg)
{
	t_param	*res;
	char	*key;

	res = new_param();
	if (res == NULL)
		return (NULL);
	if (seg->slice.len == 1 && seg->slice.begin[0] == '?')
	{
		res->value = ft_itoa(shell->last_status);
		res->len = ft_strlen(res->value);
		return (res);
	}
	key = malloc(sizeof(char) * (seg->slice.len + 1));
	if (key == NULL)
		return (free(res), NULL);
	ft_strlcpy(key, seg->slice.begin, seg->slice.len + 1);
	res->value = get_env_value(&shell->env_list, key);
	free(key);
	if (res->value == NULL)
		res->len = 0;
	else
		res->len = ft_strlen(res->value);
	return (res);
}

size_t	consume_param(t_exp *exp, t_seg *seg, char *cur)
{
	t_param	*tmp;
	size_t	len;

	tmp = exp->head;
	len = tmp->len;
	if (tmp->value != NULL)
		ft_strlcpy(cur, tmp->value, tmp->len + 1);
	exp->head = exp->head->next;
	free_param(&tmp, seg);
	if (exp->head == NULL)
		exp->tail = NULL;
	return (len);
}
*/
