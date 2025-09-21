/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 22:07:18 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/21 03:51:47 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*new_list_node_dup(char *todup)
{
	char	*content;
	t_list	*new_node;

	content = ft_strdup(todup);
	if (content == NULL)
		return (NULL);
	new_node = ft_lstnew(content);
	if (new_node == NULL)
		return (free(content), NULL);
	return (new_node);
}

int	minsortedlst_new_push(t_list **lst, char *todup)
{
	t_list	*new_node;
	t_list	*cur;

	new_node = new_list_node_dup(todup);
	if (new_node == NULL)
		return (1);
	if (*lst == NULL || ft_strcmp(new_node->content, (*lst)->content) < 0)
	{
		new_node->next = *lst;
		*lst = new_node;
	}
	else
	{
		cur = *lst;
		while (cur->next != NULL
			&& ft_strcmp(new_node->content, cur->next->content) >= 0)
			cur = cur->next;
		new_node->next = cur->next;
		cur->next = new_node;
	}
	return (0);
}
