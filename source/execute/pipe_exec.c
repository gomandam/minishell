/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/27 19:56:27 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include <sys/wait.h>

// waits for process ids, returns exit status of right child process
static int	wait_pipe(pid_t p1, pid_t p2)
{
	int	st1;	// storage exit status of processes
	int	st2;	// initialized zero will be overwritten by waitpid

	waitpid(p1, &st1, 0);
	waitpid(p2, &st2, 0);
	if (WIFEXITED(st2))
		return (WEXITSTATUS(st2));
	if (WIFSIGNALED(st2))
		return (128 + WTERMSIG(st2));
	return (1);
}

// local function for exec_ast_pipe();
// setup child pipe, execute subtree then exit
static void	child_pipe_end(t_ast *node, t_env_list *env, int end_fd, int stdio)
{
	dup2(end_fd, stdio);
	close(end_fd);
	exit(execute_ast(node, env));
}

// fork the child process for a pipe
static pid_t	pipe_fork(t_ast *node, t_env_list *env, int fd, int stdio)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		child_pipe_end(node, env, fd, stdio);
	return (pid);
}
	
// Version 2: Refactored to be norminette compliant
int	exec_ast_pipe(t_shell *shell, t_ast *node)
{
	int	fd[2];
	pid_t	pid_l;
	pidt_t	pid_r;

	if (!node || !node->u_data.op.left || !node->u_data.op.right)
		return (1);
	if (pipe(fd) < 0)
		return (perror("minishell: pipe"), 1);
	// define STDOUT_FILENO and STDIN_FILENO
	pid_l = pipe_fork(node->u_data.op.left, &shell->env_list, fd[1], STDOUT_FILENO);
	pid_r = pipe_fork(node->u_data.op.right, &shell->env_list, fd[0], STDIN_FILENO);
	close(fd[0]);
	close(fd[1]);
	return (wait_pipe(pid_l, pid_r));
}

/*	Branch build > before 27/08/2025 <pipe_exec.c>
	REFACTOR to be  norminette compliant
int	exec_ast_pipe(t_shell *shell, t_ast *node)
{
	int		fd[2];
	pid_t	pid_l;
	pid_t	pid_r;

	if (!node || !node->u_data.op.left || !node->u_data.op.right)
		return (1);
	if (pipe(fd) < 0)
		return (perror("minishell: pipe"), 1);
	pid_l = fork();
	if (pid_l == 0)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		exit(execute_ast(node->u_data.op.left, &shell->env_list));
	}
	pid_r = fork();
	if (pid_r == 0)
	{
		dup2(fd[0], 0);
		close(fd[1]);
		close(fd[0]);
		exit(execute_ast(node->u_data.op.right, &shell->env_list));
	}
	close(fd[0]);
	close(fd[1]);
	return (wait_pipe(pid_l, pid_r));
}
*/
