/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/30 20:00:03 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>

// fork and execute external cmd, wait for completion. Returns exit status
// 127 (manual) Utility to be executed was not found.
// Purpose: waitpid(pid, &status, 0); for single CMD, not pipe. Get exit code, no zombies


// 1. /bin/ls, ./sample_script.sh -> in this cases, don't need to search anything, only execve
// 		examples 'ls' command, which can be found in the environment which has a list of variables, 
// 		and inside the directory of the PATH which the commands can be seen.
// 2. ls, cat, and more commands... -> cmd: search in PATH env where is the cmd:
//   - if PATH don't exist, cmd not found
//   - if PATH exist, search in each dir, each dir in PATH is separated by ":" (colon)
//   - for each dir in PATH, you must concatenate the name of the dir, with '/', and the name of the cmd:
//      - ex: /home/angelunix/.local/bin + / + ls -> /home/angelunix/.local/bin/ls
//   - for each concatenate of the name of the cmd with each dir in PATH, 
// 				you will check if the file exists, with access(path, F_OK) function
//   - if yout find a dir path that contains the cmd, you will replace the original argv[0],
// 				with the concat dir and path you create for access
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
	return (1);
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
	return (run_external(shell, cmd));	// remove after debug
}

// expand & execute cmd AST node. returns exit status
int	exec_ast_cmd(t_shell *shell, t_cmd *cmd)
{
	if (expand_cmd(shell, cmd) != 0)
		return (1);
	return (run_builtin_external(shell, cmd));
}
