/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 18:43:52 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/24 01:25:41 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*create_env_node(char *full, char *val)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->full = full;
	node->value = val;
	node->next = NULL;
	return (node);
}

t_env	**env_find(t_env_list *env_list, const char *key, size_t keylen,
	t_env **prev_out)
{
	t_env	**cur;
	t_env	*prev;

	if (!key || *key == '\0' || !env_list || !env_list->head)
		return (NULL);
	cur = &env_list->head;
	prev = NULL;
	while (*cur != NULL)
	{
		if (ft_strncmp((*cur)->full, key, keylen) == 0
			&& (*cur)->full[keylen] == '=')
		{
			if (prev_out)
				*prev_out = prev;
			return (cur);
		}
		prev = *cur;
		cur = &(*cur)->next;
	}
	if (prev_out)
		*prev_out = prev;
	return (NULL);
}

void	env_push(t_env_list *env_list, t_env *node)
{
	if (env_list->head == NULL)
		env_list->head = node;
	else
		env_list->tail->next = node;
	env_list->tail = node;
	env_list->size++;
	if (env_list->envp != NULL)
	{
		free(env_list->envp);
		env_list->envp = NULL;
	}
}

char	**get_envp(t_shell *shell)
{
	size_t	i;
	t_env	*cur;

	if (shell->env_list.envp != NULL)
		return (shell->env_list.envp);
	shell->env_list.envp = malloc(sizeof(char *) * (shell->env_list.size + 1));
	if (shell->env_list.envp == NULL)
		return (NULL);
	cur = shell->env_list.head;
	i = 0;
	while (cur != NULL)
	{
		shell->env_list.envp[i] = cur->full;
		i++;
		cur = cur->next;
	}
	shell->env_list.envp[i] = NULL;
	return (shell->env_list.envp);
}
