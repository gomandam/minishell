/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:25 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 01:16:00 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Changed to struct state t_shell *shell -> access set_last_status()
#include "../../include/minishell.h"
#include "../../libft/libft.h"

// ft_putendl_fd() reference
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

/*
	env - lists environmental variables outputs KEY=VALUE
	stores info used by processes (e.g. PATH, HOME, USER)
	each string has "=" so validate on this
	read (RTFM!) -> man 3 getenv
*/
/* TESTS
env -i

env foo bar

env | head -n 3    or | tail -n 3

env | cat

env | (exit 1)

env > /dev/null
env > test.txt
env > /dev/full
env > .

*/
