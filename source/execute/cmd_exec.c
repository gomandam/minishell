/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 22:36:51 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/wait.h>
#include <fcntl.h>

static int	free_external(t_shell *shell, t_ast **ast, char *cmd_path)
{
	if (cmd_path)
		free(cmd_path);
	free_exp_ast(ast);
	free_env_list(&shell->env_list);
	if (shell->line)
		ft_freestr(&shell->line);
	rl_clear_history();
	return (1);
}

int	run_external(t_shell *shell, t_ast **ast, pid_t *pid)
{
	char	*cmd_path;

	cmd_path = NULL;
	if (resolve_cmd_path(shell, &cmd_path, (*ast)->u_data.cmd.u_data.argv[0]))
		return (1);
	if (pid)
	{
		*pid = fork();
		if (*pid == -1)
			return (perror("minishell: fork"), free(cmd_path), 1);
		else if (*pid != 0)
			return (free(cmd_path), 0);
	}
	signals_exec(shell);
	if (external_redirs(shell, &(*ast)->u_data.cmd.redir) == 0)
	{
		execve(cmd_path, (*ast)->u_data.cmd.u_data.argv, get_envp(shell));
		perror_execve(shell, (*ast)->u_data.cmd.u_data.argv[0]);
	}
	free_external(shell, ast, cmd_path);
	if (pid == NULL)
		return (1);
	return (exit(shell->last_status), 1);
}

// Decide builtin or external, execute builtin directly
int	run_cmd(t_shell *shell, t_ast **ast, pid_t *pid)
{
	t_cmd		*cmd;
	t_builtin	type;

	cmd = &(*ast)->u_data.cmd;
	if (!cmd->u_data.argv || !cmd->u_data.argv[0])
		return (0);
	type = get_builtin(cmd->u_data.argv[0]);
	if (type != B_CMD)
		return (run_builtin(shell, ast, pid, type));
	get_envp(shell);
	if (shell->env_list.envp == NULL)
		return (1);
	return (run_external(shell, ast, pid));
}

int	execute_ast_cmd(t_shell *shell, t_ast **cmd)
{
	pid_t	pid;

	if (!cmd || !*cmd)
		return (0);
	if (expand_cmd(shell, &(*cmd)->u_data.cmd) == 1)
		return (free_parse_ast(cmd), 1);
	pid = -1;
	if (run_cmd(shell, cmd, &pid) == 1)
		return (free_exp_ast(cmd), 1);
	free_exp_ast(cmd);
	wait_last_pid(shell, pid);
	return (0);
}
