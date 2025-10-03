/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 22:33:45 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
