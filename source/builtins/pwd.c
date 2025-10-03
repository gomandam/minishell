/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:55 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 22:29:38 by gomandam         ###   ########.fr       */
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

static void	pwd_set_status(t_shell *shell, int error)
{
	if (error)
		set_last_status(shell, 1);
	else
		set_last_status(shell, 0);
}

int	ft_pwd(t_shell *shell, int out_fd)
{
	char	*cwd;
	int		error;

	cwd = getcwd(NULL, 0);
	error = 0;
	if (cwd == NULL)
	{
		perror("minishell: pwd: error retrieving current directory");
		return (pwd_set_status(shell, 1), 1);
	}
	error = ft_putendl_check(cwd, out_fd);
	pwd_set_status(shell, error);
	free(cwd);
	return (error);
}
