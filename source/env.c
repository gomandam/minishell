/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 18:43:52 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/18 12:19:16 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**get_envp_shell(t_shell *shell)
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

// TODO: in export, if ft_strchr(full, '=') is NULL, will not execute this function, else, execute this function with the pointer + 1
t_env	*create_env_node(char *full, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (node == NULL)
		return (NULL);
	node->full = full;
	node->value = value;
	node->next = NULL;
	return (node);
}

char	*get_env_value(t_env_list *env_list, const char *key)
{
	t_env	*cur;

	cur = env_list->head;
	while (cur != NULL)
	{
		if (ft_strcmp(cur->full, key) == '=')
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}

char	*get_env_n_value(t_env_list *env_list, const char *key, size_t n)
{
	t_env	*cur;

	cur = env_list->head;
	while (cur != NULL)
	{
		if (ft_strncmp(cur->full, key, n + 1) == ('=' - key[n]))
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}

void	env_list_push(t_env_list *env_list, t_env *node)
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
