/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/27 17:30:32 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	seq_close(t_ast *ast, int in_fd, int out_fd)
{
	if (ast == NULL || ast->type == AST_CMD || ast->type == AST_SUBSH)
		return ;
	if (ast->type == AST_PIPE)
	{
		if (ast->u_data.op.pipe_fd[0] != in_fd
			&& ast->u_data.op.pipe_fd[0] != out_fd)
			ft_close(&ast->u_data.op.pipe_fd[0]);
		if (ast->u_data.op.pipe_fd[1] != in_fd
			&& ast->u_data.op.pipe_fd[1] != out_fd)
			ft_close(&ast->u_data.op.pipe_fd[1]);
	}
	seq_close(ast->u_data.op.left, in_fd, out_fd);
	seq_close(ast->u_data.op.right, in_fd, out_fd);
}

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

void	execute_seq_cmd(t_shell *shell, t_ast **cmd, int *in_fd, int *out_fd)
{
	seq_close(shell->ast, *in_fd, *out_fd);
	if (ft_dup2(in_fd, STDIN_FILENO) == 0)
	{
		if (ft_dup2(out_fd, STDOUT_FILENO) == 0)
		{
			run_builtin_external(shell, cmd, NULL);
		}
		else
			perror_syscall(shell, "minishell: dup2");
	}
	else
		perror_syscall(shell, "minishell: dup2");
	if (*in_fd != STDIN_FILENO)
		ft_close(in_fd);
	if (*out_fd != STDOUT_FILENO)
		ft_close(out_fd);
	free_exp_ast(cmd);
	free_parse_ast(&shell->ast);
	free_env_list(&shell->env_list);
	exit(shell->last_status);
}

pid_t	execute_seq_pipe(t_shell *shell, t_ast **op, int *in_fd, int *out_fd);

pid_t	execute_left_side(t_shell *shell, t_ast **op, int *in_fd)
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
		execute_seq_cmd(shell, &(*op)->u_data.op.left, in_fd,
			&(*op)->u_data.op.pipe_fd[1]);
	}
	pid = execute_seq_pipe(shell, &(*op)->u_data.op.left, in_fd,
			&(*op)->u_data.op.pipe_fd[1]);
	return (pid);
}

pid_t	execute_right_side(t_shell *shell, t_ast **op, int *out_fd)
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
		&(*op)->u_data.op.pipe_fd[0], out_fd);
	exit(shell->last_status);
	return (-1);
}

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
		return (1); // TODO: still are open fds in wrong expansion or pipe error
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
