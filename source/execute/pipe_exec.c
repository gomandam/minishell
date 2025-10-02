/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 22:40:42 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

pid_t	execute_seq_pipe(t_shell *shell, t_ast **op, int *in_fd, int *out_fd);

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
