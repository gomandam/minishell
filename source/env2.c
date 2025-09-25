/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 18:28:25 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/24 00:47:05 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	env_keylen_full(const char *full)
{
	size_t	i;

	if (!full)
		return (0);
	i = 0;
	while (full[i] && full[i] != '=')
		i++;
	return (i);
}

char	*env_get_value_keylen(t_env_list *l, const char *key, size_t keylen)
{
	t_env	**node;

	if (!key || *key == '\0' || !l || !l->head || keylen == 0)
		return (NULL);
	node = env_find(l, key, keylen, NULL);
	if (node == NULL || *node == NULL)
		return (NULL);
	return ((*node)->value);
}

char	*env_get_value(t_env_list *env_list, const char *key)
{
	size_t	keylen;

	if (!key || *key == '\0' || !env_list || !env_list->head)
		return (NULL);
	keylen = env_keylen_full(key);
	return (env_get_value_keylen(env_list, key, keylen));
}

static void	env_rm_node(t_env_list *env_list, t_env *node, t_env *prev)
{
	if (prev)
		prev->next = node->next;
	else
		env_list->head = node->next;
	if (env_list->tail == node)
		env_list->tail = prev;
	free(node->full);
	free(node);
	env_list->size--;
	if (env_list->envp)
	{
		free(env_list->envp);
		env_list->envp = NULL;
	}
}

int	env_remove(t_env_list *env_list, const char *key, size_t keylen)
{
	t_env	*cur;
	t_env	*prev;
	size_t	eq;

	cur = env_list->head;
	prev = NULL;
	while (cur != NULL)
	{
		eq = env_keylen_full(cur->full);
		if (eq == keylen && ft_strncmp(cur->full, key, keylen) == 0)
		{
			env_rm_node(env_list, cur, prev);
			return (1);
		}
		prev = cur;
		cur = cur->next;
	}
	return (0);
}
