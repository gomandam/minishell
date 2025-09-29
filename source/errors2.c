/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/26 17:25:47 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	perror_usage(t_shell *shell)
{
	write(STDERR_FILENO, "minishell: usage: ./minishell\n", 30);
	set_last_status(shell, 1);
	if (shell)
		shell->finished = 1;
	return (1);
}

int	perror_cmdnotfound(t_shell *shell, const char *cmd)
{
	size_t	cmd_len;

	cmd_len = ft_strlen(cmd);
	write(STDERR_FILENO, "minishell: command not found: ", 30);
	write(STDERR_FILENO, cmd, cmd_len);
	write(STDERR_FILENO, "\n", 1);
	set_last_status(shell, 127);
	return (1);
}

int	perror_syscall(t_shell *shell, char *msg)
{
	if (msg)
		perror(msg);
	else
		perror("minishell");
	set_last_status(shell, 1);
	if (shell)
		shell->finished = 1;
	return (1);
}

int	pwarn_heredoceof(const char *delim)
{
	size_t	delim_len;

	delim_len = ft_strlen(delim);
	write(STDERR_FILENO, "\n", 1);
	write(STDERR_FILENO,
		"minishell: warning: here-document delimited by end-of-file (wanted `",
		68);
	write(STDERR_FILENO, delim, delim_len);
	write(STDERR_FILENO, "')\n", 3);
	return (0);
}

int	perror_execve(t_shell *shell, const char *cmd)
{
	char	*msg;

	if (cmd == NULL)
		return (perror_syscall(shell, "minishell: execve"));
	else if (errno == ENOMEM)
		return (perror_malloc(shell));
	else if (errno == ENOENT)
		return (perror_cmdnotfound(shell, cmd));
	msg = ft_strjoin("minishell: ", cmd);
	if (msg == NULL)
		return (perror_malloc(shell));
	perror(msg);
	free(msg);
	if (errno == EACCES)
		set_last_status(shell, 126);
	else
		set_last_status(shell, 1);
	if (shell)
		shell->finished = 1;
	return (1);
}
