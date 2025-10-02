/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 19:31:50 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/01 13:27:14 by migugar2         ###   ########.fr       */
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

void	close_no_std(int *fd)
{
	if (fd && *fd != -1 && *fd != STDIN_FILENO && *fd != STDOUT_FILENO)
	{
		close(*fd);
		*fd = -1;
	}
}
