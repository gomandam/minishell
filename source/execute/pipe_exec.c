/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:27:24 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 00:30:32 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * FILE: pipe_exec.c
 * ============================================================================
 * PURPOSE: Pipeline execution engine - manages multi-process pipelines with
 *          proper fd handling, process synchronization, and cleanup.
 *
 * CONTROL FLOW:
 *   execute_ast_pipe
 *     └─> execute_seq_pipe (recursive for nested pipes)
 *          ├─> execute_left_side
 *          │    ├─> [if CMD/SUBSH] expand_child -> fork -> execute_seq_child
 *          │    └─> [if PIPE] execute_seq_pipe (recursive)
 *          └─> execute_right_side
 *               └─> expand_child -> fork -> execute_seq_child
 *
 *   execute_seq_child (runs in child process)
 *     ├─> seq_close (close inherited pipe fds)
 *     ├─> ft_dup2 (redirect stdin/stdout)
 *     ├─> [if CMD] run_cmd (no fork)
 *     └─> [if SUBSH] run_subsh (no fork)
 *
 * EXTERNAL USAGE: execute_ast [execute.c]
 * KEY MECHANISM: Recursive descent for left side PIPE nodes, sequential fork
 *                for CMD/SUBSH leaves. Parent manages all pipe fds.
 * =========================================================================  */

#include "minishell.h"

pid_t	execute_seq_pipe(t_shell *shell, t_ast **op, int *in_fd, int *out_fd);

/* FUNCTION: execute_seq_child
 * PURPOSE: Child process execution within pipeline, setup fds and run command.
 * PARAMS: @shell - state, @ast - CMD/SUBSH node, @in_fd input, @out_fd output
 * RETURN: Never returns (calls exit with last_status)
 * BEHAVIOR: Close inherited pipes, dup2 stdin/stdout, execute command/subshell
 * NOTE: No fork inside - direct execution; cleanup all resources before exit */
void	execute_seq_child(t_shell *shell, t_ast **ast, int *in_fd, int *out_fd)
{
	seq_close(shell->ast, *in_fd, *out_fd);
	if (ft_dup2(in_fd, STDIN_FILENO) == 0)
	{
		if (ft_dup2(out_fd, STDOUT_FILENO) == 0)
		{
			if ((*ast)->type == AST_CMD)
				run_cmd(shell, ast, NULL);
			else if ((*ast)->type == AST_SUBSH)
				run_subsh(shell, ast, 1);
		}
		else
			perror_syscall(shell, "minishell: dup2");
	}
	else
		perror_syscall(shell, "minishell: dup2");
	close_no_std(in_fd);
	close_no_std(out_fd);
	if (shell->line)
		rl_clear_history();
	ft_freestr(&shell->line);
	free_exp_ast(ast);
	free_parse_ast(&shell->ast);
	free_env_list(&shell->env_list);
	exit(shell->last_status);
}

/* FUNCTION: execute_left_side
 * PURPOSE: Execute left side of pipe - create pipe, fork/recurse as needed.
 * PARAMS: @shell - state, @op - pipe AST node, @in_fd - input fd
 * RETURN: Child PID on success, -1 on error
 * BEHAVIOR: If CMD/SUBSH: expand->fork->execute_seq_child; if PIPE: recurse
 * NOTE: Creates pipe_fd communication; write end used as out_fd for left */
pid_t	execute_left_side(t_shell *shell, t_ast **op, int *in_fd)
{
	pid_t	pid;

	if (pipe((*op)->u_data.op.pipe_fd) == -1)
		return (perror_syscall(shell, "minishell: pipe"),
			free_parse_ast(op), -1);
	if ((*op)->u_data.op.left->type == AST_CMD
		|| (*op)->u_data.op.left->type == AST_SUBSH)
	{
		if (expand_child(shell, (*op)->u_data.op.left) == 1)
			return (free_parse_ast(&(*op)->u_data.op.left), -1);
		pid = fork();
		if (pid == -1)
			return (perror_syscall(shell, "minishell: fork"),
				free_exp_ast(&(*op)->u_data.op.left), -1);
		else if (pid > 0)
			return (free_exp_ast(&(*op)->u_data.op.left), pid);
		execute_seq_child(shell, &(*op)->u_data.op.left, in_fd,
			&(*op)->u_data.op.pipe_fd[1]);
	}
	pid = execute_seq_pipe(shell, &(*op)->u_data.op.left, in_fd,
			&(*op)->u_data.op.pipe_fd[1]);
	return (pid);
}

/* FUNCTION: execute_right_side
 * PURPOSE: Execute right side of pipe - always CMD/SUBSH leaf node.
 * PARAMS: @shell - state, @op - pipe node, @out_fd - output fd
 * RETURN: Child PID on success, -1 on error
 * BEHAVIOR: Expand->fork->execute_seq_child with pipe read end as stdin
 * NOTE: Right side always terminates pipe chain (no recursion)	*/
pid_t	execute_right_side(t_shell *shell, t_ast **op, int *out_fd)
{
	pid_t	pid;

	if (expand_child(shell, (*op)->u_data.op.right) == 1)
		return (free_parse_ast(&(*op)->u_data.op.right), -1);
	pid = fork();
	if (pid == -1)
		return (perror_syscall(shell, "minishell: fork"),
			free_exp_ast(&(*op)->u_data.op.right), -1);
	else if (pid > 0)
		return (free_exp_ast(&(*op)->u_data.op.right), pid);
	execute_seq_child(shell, &(*op)->u_data.op.right,
		&(*op)->u_data.op.pipe_fd[0], out_fd);
	return (-1);
}

/* FUNCTION: execute_seq_pipe
 * PURPOSE: Recursive pipeline builder - handles nested pipes sequentially.
 * PARAMS: @shell - state, @op - pipe node, @in_fd - input, @out_fd - output
 * RETURN: PID of rightmost (last) child, -1 on error
 * BEHAVIOR: Execute left (recurse if PIPE), close write end, execute right
 * NOTE: Parent closes all intermediate pipe fds; returns last PID for wait */
pid_t	execute_seq_pipe(t_shell *shell, t_ast **op, int *in_fd, int *out_fd)
{
	pid_t	left;
	pid_t	last;

	left = execute_left_side(shell, op, in_fd);
	if (*in_fd != STDIN_FILENO)
		ft_close(in_fd);
	ft_close(&(*op)->u_data.op.pipe_fd[1]);
	if (left == -1)
		return (free_parse_ast(op), ft_close(&(*op)->u_data.op.pipe_fd[0]), -1);
	last = execute_right_side(shell, op, out_fd);
	ft_close(&(*op)->u_data.op.pipe_fd[0]);
	if (last == -1 && left > 0)
		waitpid(left, NULL, 0);
	return (free_parse_ast(op), last);
}

/* FUNCTION: execute_ast_pipe
 * PURPOSE: Pipeline execution entry point - setup and wait for all children.
 * PARAMS: @shell - state, @node - pipe AST root
 * RETURN: 0 on success, 1 on error
 * BEHAVIOR: Call execute_seq_pipe, wait for last child, wait for remaining
 * NOTE: wait_count tracks all forked processes; last child determines status  */
int	execute_ast_pipe(t_shell *shell, t_ast **node)
{
	pid_t	last;
	int		in_fd;
	int		out_fd;
	int		errno_cpy;
	size_t	count;

	in_fd = STDIN_FILENO;
	out_fd = STDOUT_FILENO;
	count = (*node)->u_data.op.wait_count;
	last = execute_seq_pipe(shell, node, &in_fd, &out_fd);
	if (last == -1)
		return (1);
	errno_cpy = errno;
	wait_last_pid(shell, last);
	while (count > 1)
	{
		if (wait(NULL) == -1)
			break ;
		count--;
	}
	errno = errno_cpy;
	return (0);
}
