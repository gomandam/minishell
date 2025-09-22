/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 18:28:25 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/22 20:20:46 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_env_list(t_env_list *env_list)
{
	t_env	*cur;
	t_env	*next;

	cur = env_list->head;
	while (cur != NULL)
	{
		free(cur->full);
		cur->full = NULL;
		cur->value = NULL;
		next = cur->next;
		free(cur);
		cur = next;
	}
	env_list->head = NULL;
	env_list->tail = NULL;
	env_list->size = 0;
	if (env_list->envp != NULL)
	{
		free(env_list->envp);
		env_list->envp = NULL;
	}
}

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
	node = env_find(l, key, keylen);
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
