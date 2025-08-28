/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/28 22:15:58 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>

// Fork and execute external cmd, wait for completion. Returns exit status
static int	run_external(t_shell *shell, t_cmd *cmd)
{
	int	status;
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return (perror("minishell: fork"), 1);
	if (pid == 0)
	{
		execve(cmd->u_data.argv[0], cmd->u_data.argv, shell->env_list.envp);
		perror("minishell: execve");
		_exit(127);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);	// ? Return PID 
}

// Decide builtin or external, execute builtin directly, and 
// external is via fork/execve. Then, return cmd exit status
int	run_builtin_external(t_shell *shell, t_cmd *cmd)
{
	(void)shell; 					// remove after debug
	if (!cmd->u_data.argv || !cmd->u_data.argv[0])
		return (0);
//	if (is_builtin(cmd->u_data.argv[0]))
//		return (run_builtin(shell, cmd->u_data.argv));
//	return (run_external(shell, cmd));
	if (is_builtin(cmd->u_data.argv[0]))	// remove codesnippet after debug
		return ((debug_builtin(cmd->u_data.argv[0])), 0);
	return (0);					// remove after debug
}

// expand & execute cmd AST node. returns exit status
int	exec_ast_cmd(t_shell *shell, t_cmd *cmd)
{
	if (expand_cmd(shell, cmd) != 0)
		return (1);
	return (run_builtin_external(shell, cmd));
}
