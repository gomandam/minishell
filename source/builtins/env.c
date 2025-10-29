/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:25 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/29 17:52:15 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* PURPOSE: Print environment in KEY=VALUE lines (only entries containing '=').
 * CONTROL FLOW:
 *   1) Iterate env_list
 *   2) Print lines with '=' and trailing newline
 *   3) Set last_status 0 on success, 1 if any write fails
 * RELATIONSHIPS: Used by run_builtin (builtin_exec.c).
 * EXTERNAL USAGE: builtin "env".
 * NOTES: Ignores entries without '='. Writes to provided FD.
 * ========================================================================== */
#include "minishell.h"
/* PURPOSE: Print s + newline atomically; check write counts.
 * PARAMS: @s,@fd	  RETURN: 0 ok, 1 on error. */
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

/* PURPOSE: Update last_status based on error. */
static void	env_set_status(t_shell *shell, int error)
{
	if (error)
		set_last_status(shell, 1);
	else
		set_last_status(shell, 0);
}

/* PURPOSE: Entry point for "env".	PARAMS: @shell,@out_fd
 * RETURN: 0 ok, 1 if any write failed. */
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
