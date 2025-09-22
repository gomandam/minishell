/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/22 16:05:19 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

void	export_perror_identifier(t_shell *shell, char *argv)
{
	ft_putstr_fd("minishell: export: '", 2);
	ft_putstr_fd(argv, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	set_last_status(shell, 1);
}

static int	cmp_exportkey(const char *s1, const char *s2, size_t eq_s1)
{
	size_t	eq_s2;
	size_t	minlen;
	size_t	valcmp;

	eq_s2 = 0;
	while (s2[eq_s2] && s2[eq_s2] != '=')
		eq_s2++;
	if (eq_s1 <= eq_s2)
		minlen = eq_s1;
	else
		minlen = eq_s2;
	valcmp = ft_strncmp(s1, s2, minlen);
	if (valcmp != 0)
		return (valcmp);
	if (eq_s1 == eq_s2)
		return (0);
	if (eq_s1 < eq_s2)
		return (-1);
	return (1);
}

static int	minsorted_key_push(t_list **lst, char *str)
{
	size_t	eq;
	t_list	*new_node;
	t_list	*cur;

	new_node = ft_lstnew(str);
	if (new_node == NULL)
		return (1);
	eq = ft_strchr(str, '=') - str;
	if (*lst == NULL
		|| cmp_exportkey(new_node->content, (*lst)->content, eq) < 0)
	{
		new_node->next = *lst;
		*lst = new_node;
	}
	else
	{
		cur = *lst;
		while (cur->next != NULL
			&& cmp_exportkey(new_node->content, cur->next->content, eq) >= 0)
			cur = cur->next;
		new_node->next = cur->next;
		cur->next = new_node;
	}
	return (0);
}

int	export_print_all(t_shell *shell)
{
	t_env	*cur;
	t_list	*lst;
	t_list	*cur_lst;

	cur = shell->env_list.head;
	lst = NULL;
	while (cur)
	{
		if (minsorted_key_push(&lst, cur->full) == 1)
			return (perror_malloc(shell), ft_lstclear(&lst, NULL), 1);
		cur = cur->next;
	}
	cur_lst = lst;
	while (cur_lst)
	{
		write(1, cur_lst->content, ft_strlen(cur_lst->content));
		write(1, "\n", 1);
		cur_lst = cur_lst->next;
	}
	ft_lstclear(&lst, NULL);
	set_last_status(shell, 0);
	return (0);
}
