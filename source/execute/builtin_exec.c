/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:03:54 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 00:33:16 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * FILE: builtin_exec.c
 * ============================================================================
 * PURPOSE: Builtin command dispatcher - identifies and executes shell builtins
 *          with proper redirection handling.
 *
 * CONTROL FLOW:
 *   get_builtin (string -> enum mapping)
 *   run_builtin
 *     ├─> resolve_redirs (redirections.c)
 *     ├─> ft_exit/pwd/env/unset/echo/export/cd (builtins/ *.c)
 *     └─> close_no_std (cleanup fds)
 *
 * BUILTINS:
 *   1. echo   - Print arguments [builtins/echo.c]
 *   2. cd     - Change directory [builtins/cd.c]
 *   3. pwd    - Print working directory [builtins/pwd.c]
 *   4. export - Set environment variables [builtins/export.c]
 *   5. unset  - Unset environment variables [builtins/unset.c]
 *   6. env    - Print environment [builtins/env.c]
 *   7. exit   - Exit shell [builtins/exit.c]
 *
 * EXTERNAL USAGE: run_cmd [cmd_exec.c]
 * NOTE: Builtins execute in parent process, can modify shell state
 * =========================================================================== */
#include "minishell.h"

/* FUNCTION: get_builtin
 * PURPOSE: Map command string to builtin enum type.
 * PARAMS: @cmd - command name string
 * RETURN: t_builtin enum (B_CMD if not builtin)
 * NOTE: Case-sensitive exact match required  */
t_builtin	get_builtin(char *cmd)
{
	if (!cmd)
		return (B_CMD);
	if (ft_strcmp(cmd, "echo") == 0)
		return (B_ECHO);
	if (ft_strcmp(cmd, "cd") == 0)
		return (B_CD);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (B_PWD);
	if (ft_strcmp(cmd, "export") == 0)
		return (B_EXPORT);
	if (ft_strcmp(cmd, "unset") == 0)
		return (B_UNSET);
	if (ft_strcmp(cmd, "env") == 0)
		return (B_ENV);
	if (ft_strcmp(cmd, "exit") == 0)
		return (B_EXIT);
	return (B_CMD);
}

/* FUNCTION: run_builtin
 * PURPOSE: Execute builtin command with redirections in parent process.
 * PARAMS: @shell - state, @ast - command AST, @pid - set to 0, @type - builtin
 * RETURN: Builtin execution status
 * BEHAVIOR: Resolve redirections to fds[2], dispatch to builtin, cleanup fds
 * NOTE: Sets *pid=0 to signal no child process; fds[0]=in, fds[1]=out  */
int	run_builtin(t_shell *shell, t_ast **ast, pid_t *pid, t_builtin type)
{
	int	status;
	int	fds[2];

	if (resolve_redirs(shell, &(*ast)->u_data.cmd.redir, fds) != 0)
		return (1);
	if (type == B_EXIT)
		status = ft_exit(shell, ast);
	else if (type == B_PWD)
		status = ft_pwd(shell, fds[1]);
	else if (type == B_ENV)
		status = ft_env(shell, fds[1]);
	else if (type == B_UNSET)
		status = ft_unset(shell, (*ast)->u_data.cmd.u_data.argv);
	else if (type == B_ECHO)
		status = ft_echo(shell, &(*ast)->u_data.cmd, fds[1]);
	else if (type == B_EXPORT)
		status = ft_export(shell, (*ast)->u_data.cmd.u_data.argv, fds[1]);
	else
		status = ft_cd(shell, (*ast)->u_data.cmd.u_data.argv);
	if (pid != NULL)
		*pid = 0;
	close_no_std(&fds[0]);
	close_no_std(&fds[1]);
	return (status);
}
