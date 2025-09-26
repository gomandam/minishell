/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/27 00:25:41 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
// TODO: last_status write
// waits for process ids, returns exit status of right child process
// st1 & st2: storage exit status, overwritten by waitpid even initialized 0
static int	wait_pipe(t_shell *shell, pid_t p1, pid_t p2)
{
	int	st2;
	int	sig;

	signals_wait(shell);
	if (p2 != -1)
		waitpid(p2, &st2, 0);
	if (p1 != -1)
		waitpid(p1, NULL, 0);
	signals_repl(shell);
	if (WIFEXITED(st2))
		set_last_status(shell, WEXITSTATUS(st2));
	else if (WIFSIGNALED(st2))
	{
		sig = WTERMSIG(st2);
		if (sig == SIGQUIT)
		{
			if (__WCOREDUMP(st2))
				write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
			else
				write(STDOUT_FILENO, "Quit\n", 5);
		}
		set_last_status(shell, 128 + WTERMSIG(st2));
	}
	else
		set_last_status(shell, 1);
	return (0);
}

// local function for execute_ast_pipe();
// setup child pipe, execute subtree then exit
// ! TODO: Control syscall errors
static void	child_pipe_end(t_shell *shell, t_ast **node, int fd[2], int stdio)
{
	ft_putstr_fd("DEBUG: entered child_pipe_end at pipe_exec.c\n", 2);
	int	in_fd;
	int	close_fd;

	in_fd = -1;
	close_fd = -1;
	if (stdio == STDOUT_FILENO)
	{
		in_fd = fd[1];
		close_fd = fd[0];
	}
	else if (stdio == STDIN_FILENO)
	{
		in_fd = fd[0];
		close_fd = fd[1];
	}
	if (close_fd >= 0)
		close(close_fd);
	if (in_fd >= 0)
	{
		dup2(in_fd, stdio);
		close(in_fd);
	}
	exit(execute_ast(shell, node));
}

// fork the child process for a pipe
// All commands in the pipeline execute in parallel,
// 	regardless of pipeline depth or tree structure.
static pid_t	pipe_fork(t_shell *shell, t_ast **node, int fd[2], int stdio)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		child_pipe_end(shell, node, fd, stdio);
	return (pid);
}

// Per-node: create pipe, fork: L | R, close fds, execute concurrent, return right status.
// fork: left writes fd[1] and fork: right reads fd[0]
// Each stage (AST_PIPE) immediately forks both left and right children.
int	execute_ast_pipe(t_shell *shell, t_ast **node)
{
	int		fd[2];
	pid_t	pid_l;
	pid_t	pid_r;
	int		status;

	fd[0] = -1;
	fd[1] = -1;
	/ if (!node || !node->u_data.op.left || !node->u_data.op.right)
		return (1); /
	if (pipe(fd) == -1)
		return (perror_syscall(shell, "minishell: pipe"),
			free_parse_ast(node), 1); // TODO: last_status
	pid_l = pipe_fork(shell, &(*node)->u_data.op.left, fd,
			STDOUT_FILENO);
	ft_close(&fd[1]);
	pid_r = pipe_fork(shell, &(*node)->u_data.op.right, fd,
			STDIN_FILENO);
	ft_close(&fd[0]);
	status = wait_pipe(shell, pid_l, pid_r);
	free_parse_ast(node);
	return (status);
}
*/

void	wait_pipe(t_shell *shell, size_t count, pid_t last)
{
	size_t	i;
	int		errno_cpy;

	errno_cpy = errno;
	wait_last_pid(shell, last);
	i = 1;
	while (i < count)
	{
		if (wait(NULL) == -1)
			break ;
		i++;
	}
	errno = errno_cpy;
}

void	execute_seq_cmd(t_shell *shell, t_ast **cmd, int in_fd, int out_fd)
{
	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			perror_syscall(shell, "minishell: dup2"); // TODO
			ft_close(&in_fd);
			if (out_fd != STDOUT_FILENO)
				ft_close(&out_fd);
			free_exp_ast(cmd);
			free_parse_ast(&shell->ast);
			free_env_list(&shell->env_list);
			exit(shell->last_status);
		}
		ft_close(&in_fd);
	}
	if (out_fd != STDOUT_FILENO)
	{
		if (dup2(out_fd, STDOUT_FILENO) == -1)
		{
			perror_syscall(shell, "minishell: dup2"); // TODO
			ft_close(&out_fd);
			free_exp_ast(cmd);
			free_parse_ast(&shell->ast);
			free_env_list(&shell->env_list);
			exit(shell->last_status);
		}
		ft_close(&out_fd);
	}
	run_builtin_external(shell, cmd, NULL); // TODO: consider subshell too
	free_exp_ast(cmd);
	free_parse_ast(&shell->ast);
	free_env_list(&shell->env_list);
	exit(shell->last_status);
}

pid_t	execute_seq_pipe(t_shell *shell, t_ast **op, int in_fd, int out_fd);

pid_t	execute_left_side(t_shell *shell, t_ast **op, int in_fd)
{
	pid_t	pid;

	if (pipe((*op)->u_data.op.pipe_fd) == -1)
		return (perror_syscall(shell, "minishell: pipe"),
			free_parse_ast(op), -1);
	if ((*op)->u_data.op.left->type == AST_CMD)
	{
		if (expand_cmd(shell, &(*op)->u_data.op.left->u_data.cmd) == 1)
			return (free_parse_ast(&(*op)->u_data.op.left), -1);
		pid = fork();
		if (pid == -1)
			return (perror_syscall(shell, "minishell: fork"),
				free_exp_ast(&(*op)->u_data.op.left), -1);
		else if (pid > 0)
			return (free_exp_ast(&(*op)->u_data.op.left), pid);
		ft_close(&(*op)->u_data.op.pipe_fd[0]);
		execute_seq_cmd(shell, &(*op)->u_data.op.left, in_fd,
			(*op)->u_data.op.pipe_fd[1]);
	}
	pid = execute_seq_pipe(shell, &(*op)->u_data.op.left, in_fd,
			(*op)->u_data.op.pipe_fd[1]);
	return (pid);
}

pid_t	execute_right_side(t_shell *shell, t_ast **op, int out_fd)
{
	pid_t	pid;

	if (expand_cmd(shell, &(*op)->u_data.op.right->u_data.cmd) == 1)
		return (free_parse_ast(&(*op)->u_data.op.right), -1);
	pid = fork();
	if (pid == -1)
		return (perror_syscall(shell, "minishell: fork"),
			free_exp_ast(&(*op)->u_data.op.right), -1);
	else if (pid > 0)
		return (free_exp_ast(&(*op)->u_data.op.right), pid);
	execute_seq_cmd(shell, &(*op)->u_data.op.right,
		(*op)->u_data.op.pipe_fd[0], out_fd);
	exit(shell->last_status);
	return (-1);
}

pid_t	execute_seq_pipe(t_shell *shell, t_ast **op, int in_fd, int out_fd)
{
	printf("DEBUG: Entered execute_seq_pipe(); in_fd: %d out_fd: %d \n", in_fd, out_fd);
	pid_t	left;
	pid_t	last;

	left = execute_left_side(shell, op, in_fd);
	if (in_fd != STDIN_FILENO)
	{
		printf("ft_close in_fd: %d \n", in_fd);
		ft_close(&in_fd);
	}
	ft_close(&(*op)->u_data.op.pipe_fd[1]);
	if (left == -1)
		return (free_parse_ast(op), ft_close(&(*op)->u_data.op.pipe_fd[0]), -1);
	last = execute_right_side(shell, op, out_fd);
	ft_close(&(*op)->u_data.op.pipe_fd[0]);
	if (last == -1 && left > 0)
		waitpid(left, NULL, 0);
	return (free_parse_ast(op), last);
}

int	execute_ast_pipe(t_shell *shell, t_ast **node)
{
	ft_putendl_fd("DEBUG: Entered execute_ast_pipe(); ", 2);
	pid_t	last;
	size_t	count;

	count = (*node)->u_data.op.wait_count;
	last = execute_seq_pipe(shell, node, STDIN_FILENO, STDOUT_FILENO);
	if (last == -1)
		return (1); // TODO
	wait_pipe(shell, count, last);
	return (0);
}
