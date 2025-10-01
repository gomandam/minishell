/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/01 16:56:54 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>

// TODO: last_status write
// fork and execute external cmd, wait for completion. Returns exit status
// 127 (manual) Utility to be executed was not found.
// waitpid(pid, &status, 0); for single CMD, not pipe. Get exit code, no zombies
static int	run_external(t_shell *shell, t_ast **ast, pid_t *pid)
{
	char	*cmd_path;
	char	**envp;

	envp = get_envp(shell);
	if (envp == NULL)
		return (1);
	cmd_path = NULL;
	if (resolve_cmd_path(shell, &cmd_path, (*ast)->u_data.cmd.u_data.argv[0]))
		return (1);
	if (pid)
	{
		*pid = fork();
		if (pid && *pid == -1)
			return (perror("minishell: fork"), free(cmd_path), 1);
		else if (pid && *pid != 0)
			return (free(cmd_path), 0);
	}
	signals_exec(shell);
	// TODO: redirections here
	execve(cmd_path, (*ast)->u_data.cmd.u_data.argv, envp);
	perror_execve(shell, (*ast)->u_data.cmd.u_data.argv[0]);
	free(cmd_path);
	free_exp_ast(ast);
	free_env_list(&shell->env_list);
	exit(shell->last_status);
	return (1);
}

// Decide builtin or external, execute builtin directly, and
// external is via fork/execve. Then, return cmd exit status
// TO DO: review 'unset' but so far it is okay
int	run_builtin_external(t_shell *shell, t_ast **ast, pid_t *pid)
{
	ft_putstr_fd("DEBUG: entered the run_builtin_external(); at cmd_exec.c\n", 2);
	t_cmd	*cmd;

	cmd = &(*ast)->u_data.cmd;
	if (!cmd->u_data.argv || !cmd->u_data.argv[0])
		return (0);
	if (is_builtin(cmd->u_data.argv[0]))
	{
		if (pid)
			*pid = 0;
		ft_putstr_fd("DEBUG: inside is_builtin() -> builtin fx\n", 2);
		if (!ft_strcmp(cmd->u_data.argv[0], "pwd"))
			return (ft_pwd());
		if (!ft_strcmp(cmd->u_data.argv[0], "env"))
			return (ft_env(shell));
		if (!ft_strcmp(cmd->u_data.argv[0], "unset"))
			return (ft_unset(shell, cmd->u_data.argv));
		if (!ft_strcmp(cmd->u_data.argv[0], "echo"))
			return (ft_echo(shell, cmd));
		if (!ft_strcmp(cmd->u_data.argv[0], "exit"))
			return (ft_exit(shell, cmd->u_data.argv));
		if (!ft_strcmp(cmd->u_data.argv[0], "export"))
			return (ft_export(shell, cmd->u_data.argv));
		if (!ft_strcmp(cmd->u_data.argv[0], "cd"))
			return (ft_cd(shell, cmd->u_data.argv));
	}
	return (run_external(shell, ast, pid));
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

// expand & execute cmd AST node. returns exit status
// TODO: receive t_ast **node, can free and set to NULL
// TODO: open and read redirections
int	execute_ast_cmd(t_shell *shell, t_ast **cmd)
{
	ft_putstr_fd("DEBUG: Entered execute_ast_cmd(); \n", 2);
	pid_t	pid;

	if (!cmd || !*cmd)
		return (0);
	if (expand_cmd(shell, &(*cmd)->u_data.cmd) == 1)
		return (free_parse_ast(cmd), 1);
	pid = -1;
	if (run_builtin_external(shell, cmd, &pid) == 1)
		return (free_exp_ast(cmd), 1);
	free_exp_ast(cmd);
	wait_last_pid(shell, pid);
	return (0);
}
