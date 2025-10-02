/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:25 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 22:22:17 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_putendl_check(char *s, int fd)
{
	int	len;
	int	written;

	if (s == NULL)
		return (0);
	len = 0;
	while (s[len])
		len++;
	written = write(fd, s, len);
	if (written != len)
		return (1);
	written = write(fd, "\n", 1);
	if (written != 1)
		return (1);
	return (0);
}

static void	env_set_status(t_shell *shell, int error)
{
	if (error)
		set_last_status(shell, 1);
	else
		set_last_status(shell, 0);
}

int	ft_env(t_shell *shell, int out_fd)
{
	t_env	*current;
	int		error;

	current = shell->env_list.head;
	error = 0;
	while (current)
	{
		if (current->full && ft_strchr(current->full, '='))
			if (ft_putendl_check(current->full, out_fd))
				error = 1;
		current = current->next;
	}
	env_set_status(shell, error);
	return (error);
}
