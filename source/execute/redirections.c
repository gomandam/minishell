/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:34:05 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 00:36:58 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * FILE: redirections.c
 * ============================================================================
 * PURPOSE: I/O redirection processor - handles all redirection types with
 *          proper file opening, descriptor management, and error handling.
 *
 * CONTROL FLOW:
 *   resolve_redirs (parent process, builtins)
 *     └─> open_redirection (for each redir)
 *          └─> open() with appropriate flags
 *
 *   external_redirs (child process, external commands)
 *     ├─> resolve_redirs
 *     └─> ft_dup2 (redirect stdin/stdout)
 *
 * REDIRECTION TYPES:
 *   - R_INFILE  (<)   : O_RDONLY
 *   - R_OUTFILE (>)   : O_WRONLY | O_CREAT | O_TRUNC  (0644)
 *   - R_APPEND  (>>)  : O_WRONLY | O_CREAT | O_APPEND (0644)
 *   - R_HEREDOC (<<)  : Uses pre-opened pipe fd from parser
 *
 * EXTERNAL USAGE: run_builtin [builtin_exec.c], run_external [cmd_exec.c],
 *                 execute_seq_child [pipe_exec.c], run_subsh [subsh_exec.c]
 * NOTE: Last redirection of each type (in/out) takes precedence
 * ========================================================================== */

#include "minishell.h"

/* FUNCTION: open_redirection
 * PURPOSE: Open single redirection file with appropriate flags and mode.
 * PARAMS: @shell - shell state, @redir - redirection node to open
 * RETURN: 0 on success, 1 on error
 * BEHAVIOR: Sets redir->fd; heredocs skip (already open); handles open errors
 * NOTE: Permissions 0644 for output files (rw-r--r--)  */
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

/* FUNCTION: resolve_redirs
 * PURPOSE: Process redirection list, open files, set final stdin/stdout fds.
 * PARAMS: @shell - state, @redirs - list of redirections, @fds - output [in,out]
 * RETURN: 0 on success, 1 on error
 * BEHAVIOR: Iterate list, open each, close previous, keep last of each type
 * NOTE: fds[0]=final stdin, fds[1]=final stdout; frees processed redirections */
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

/* FUNCTION: external_redirs
 * PURPOSE: Apply redirections in child process before execve.
 * PARAMS: @shell - shell state, @redirs - redirection list
 * RETURN: 0 on success, 1 on error
 * BEHAVIOR: Call resolve_redirs, dup2 fds to stdin/stdout
 * NOTE: Used only in child processes; replaces standard fds permanently */
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
