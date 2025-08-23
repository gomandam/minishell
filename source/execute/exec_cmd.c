/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/22 04:44:02 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include <sys/wait.h>

static int	run_builtin_external(t_shell *shell, t_cmd *cmd)
{
	int	status;
	pid_t	pid;

	if (!cmd->u.data.argv || !cmd->u_data.argv[0])
		return (0);
	//if (is_builtin(cmd->u.data _____ ))	// ! TO DO: is_builtin() conditional statments or control flow
	//	return (exec_builtin());	// ! TO DO: implement execute builtin
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
	return (1);	// Return PID
}

int	exec_ast_cmd(t_shell *shell, t_cmd *cmd)
{
	if (expand_cmd(shell, cmd) != 0)
		return (1);
	// ? Check if it is a is_builtin(); is on the current process and command in fork 
	return (run_builtin_external(shell, cmd));
}
