/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:48:38 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 01:26:39 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * FILE: utils_exec.c
 * ============================================================================
 * PURPOSE: Execution utilities - process synchronization, fd management, and
 *          signal handling for child processes.
 * FUNCTIONS:
 *   1. seq_close      - Close inherited pipe fds in child (recursive)
 *   2. wait_last_pid  - Wait for child and extract exit status with signals
 *   3. ft_dup2        - Safe dup2 wrapper with auto-close
 *   4. close_no_std   - Close fd if not stdin/stdout/stderr
 * EXTERNAL USAGE: Throughout execute directory
 * NOTE: wait_last_pid handles SIGINT/SIGQUIT properly with status codes
 * ========================================================================== */

#include "minishell.h"

/* FUNCTION: seq_close
 * PURPOSE: Recursively close all inherited pipe fds in child process.
 * PARAMS: @ast - AST root, @in_fd - keep open (stdin), @out_fd - keep open (stdout)
 * RETURN: void
 * BEHAVIOR: Traverse AST, close all pipe_fd[0/1] except in_fd/out_fd
 * NOTE: Prevents fd leaks in pipeline children; recursive for nested structures */
void	seq_close(t_ast *ast, int in_fd, int out_fd)
{
	if (ast == NULL || ast->type == AST_CMD)
		return ;
	if (ast->type == AST_PIPE)
	{
		if (ast->u_data.op.pipe_fd[0] != in_fd
			&& ast->u_data.op.pipe_fd[0] != out_fd)
			ft_close(&ast->u_data.op.pipe_fd[0]);
		if (ast->u_data.op.pipe_fd[1] != in_fd
			&& ast->u_data.op.pipe_fd[1] != out_fd)
			ft_close(&ast->u_data.op.pipe_fd[1]);
		seq_close(ast->u_data.op.left, in_fd, out_fd);
		seq_close(ast->u_data.op.right, in_fd, out_fd);
	}
	else if (ast->type == AST_AND_IF || ast->type == AST_OR_IF)
	{
		seq_close(ast->u_data.op.left, in_fd, out_fd);
		seq_close(ast->u_data.op.right, in_fd, out_fd);
	}
	else if (ast->type == AST_SUBSH)
		seq_close(ast->u_data.subsh.child, in_fd, out_fd);
}

/* FUNCTION: wait_last_pid
 * PURPOSE: Wait for child process and extract exit status with signal handling.
 * PARAMS: @shell - shell state, @pid - child process id
 * RETURN: void (sets shell->last_status)
 * BEHAVIOR: waitpid, handle WIFEXITED, WIFSIGNALED (SIGQUIT message), set status
 * NOTE: Status codes: normal exit, 128+signal for killed, 1 for unknown  */
void	wait_last_pid(t_shell *shell, pid_t pid)
{
	int	status;
	int	sig;

	if (pid <= 0)
		return ;
	signals_wait(shell);
	waitpid(pid, &status, 0);
	signals_repl(shell);
	if (WIFEXITED(status))
		set_last_status(shell, WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGQUIT)
		{
			if (__WCOREDUMP(status))
				write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
			else
				write(STDOUT_FILENO, "Quit\n", 5);
		}
		set_last_status(shell, 128 + WTERMSIG(status));
	}
	else
		set_last_status(shell, 1);
}

/* FUNCTION: ft_dup2
 * PURPOSE: Safe dup2 wrapper - duplicates fd only if different from target.
 * PARAMS: @oldfd - source fd pointer, @newfd - target fd
 * RETURN: 0 on success, -1 on error
 * BEHAVIOR: If oldfd!=newfd and not std, dup2 and close oldfd
 * NOTE: Automatically closes oldfd after successful dup2  */
int	ft_dup2(int *oldfd, int newfd)
{
	int	status;

	if (*oldfd != newfd && *oldfd != STDIN_FILENO && *oldfd != STDOUT_FILENO)
	{
		status = dup2(*oldfd, newfd);
		ft_close(oldfd);
		if (status == -1)
			return (-1);
	}
	return (0);
}

/* FUNCTION: close_no_std
 * PURPOSE: Safely close fd if it's not a standard fd (0,1,2) or -1.
 * PARAMS: @fd - file descriptor pointer
 * RETURN: void
 * BEHAVIOR: Close and set to -1 if valid non-standard fd
 * NOTE: Prevents accidental closure of stdin/stdout/stderr  */
void	close_no_std(int *fd)
{
	if (fd && *fd != -1 && *fd != STDIN_FILENO && *fd != STDOUT_FILENO)
	{
		close(*fd);
		*fd = -1;
	}
}
