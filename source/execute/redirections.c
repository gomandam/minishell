/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 11:49:57 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/01 13:46:24 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	open_redirection(t_shell *shell, t_redir *redir)
{
	if (redir->type == R_HEREDOC)
		return (0);
	else if (redir->type == R_INFILE)
		redir->fd = open(redir->u_data.name, O_RDONLY);
	else if (redir->type == R_OUTFILE)
		redir->fd = open(redir->u_data.name, O_WRONLY | O_CREAT | O_TRUNC,
				0644);
	else if (redir->type == R_APPEND)
		redir->fd = open(redir->u_data.name, O_WRONLY | O_CREAT | O_APPEND,
				0644);
	if (redir->fd == -1)
		return (perror_open(shell, redir->u_data.name));
	return (0);
}

int	resolve_redirs(t_shell *shell, t_redirs *redirs, int fds[2])
{
	t_redir	*next;

	fds[0] = STDIN_FILENO;
	fds[1] = STDOUT_FILENO;
	while (redirs->head != NULL)
	{
		if (open_redirection(shell, redirs->head) != 0)
			return (close_no_std(&fds[1]), close_no_std(&fds[0]), 1);
		if (redirs->head->type == R_HEREDOC || redirs->head->type == R_INFILE)
		{
			close_no_std(&fds[0]);
			fds[0] = redirs->head->fd;
		}
		else if (redirs->head->type == R_OUTFILE
			|| redirs->head->type == R_APPEND)
		{
			close_no_std(&fds[1]);
			fds[1] = redirs->head->fd;
		}
		next = redirs->head->next;
		free_exp_redir(&redirs->head, 0);
		redirs->head = next;
	}
	redirs->tail = NULL;
	return (0);
}

int	external_redirs(t_shell *shell, t_redirs *redirs)
{
	int	fd[2];

	if (resolve_redirs(shell, redirs, fd) != 0)
		return (1);
	if (ft_dup2(&fd[0], STDIN_FILENO) != 0)
		return (1);
	if (ft_dup2(&fd[1], STDOUT_FILENO) != 0)
		return (1);
	return (0);
}
