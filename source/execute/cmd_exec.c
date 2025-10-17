/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:16:10 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 00:19:56 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * FILE: cmd_exec.c
 * ============================================================================
 * PURPOSE: Command execution core - handles builtin/external command dispatch,
 *          process creation, and command path resolution.
 *
 * CONTROL FLOW:
 *   execute_ast_cmd
 *     ├─> expand_cmd (expansion/command.c)
 *     ├─> run_cmd
 *     │    ├─> get_builtin (builtin_exec.c)
 *     │    ├─> run_builtin (builtin_exec.c) [if builtin]
 *     │    └─> run_external [if external]
 *     │         ├─> resolve_cmd_path (cmd_path.c)
 *     │         ├─> fork()
 *     │         └─> [child] signals_exec -> external_redirs -> execve
 *     └─> wait_last_pid (utils_exec.c)
 *
 * EXTERNAL USAGE: execute_ast [execute.c], execute_seq_child [pipe_exec.c]
 * =========================================================================== */

#include "minishell.h"
#include <sys/wait.h>
#include <fcntl.h>

/* FUNCTION: free_external
 * PURPOSE: Child process cleanup before exit after execve failure.
 * PARAMS: @shell - shell state, @ast - expanded AST, @cmd_path - resolved path
 * RETURN: Always 1 (failure indicator)
 * NOTE: Only called in child process context before exit() */
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

/* FUNCTION: run_external
 * PURPOSE: Fork and execute external command with execve.
 * PARAMS: @shell - shell state, @ast - command AST with argv, @pid - output PID
 * RETURN: 0 in parent success, 1 on error; never returns in child
 * BEHAVIOR: Resolves command path, forks (if pid!=NULL), child execs command
 * ERROR: Command not found (127), fork failure, execve failure
 * NOTE: If pid==NULL, no fork occurs (used in pipe sequences)  */
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

/* FUNCTION: run_cmd
 * PURPOSE: Dispatcher - determines builtin vs external and executes accordingly.
 * PARAMS: @shell - shell state, @ast - command AST, @pid - output PID pointer
 * RETURN: Execution status from builtin/external handler
 * BEHAVIOR: Empty commands return 0; builtins exec in parent; externals fork
 * NOTE: Builtins modify parent shell state; externals run in child process  */
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

/* FUNCTION: execute_ast_cmd
 * PURPOSE: Main command execution entry - orchestrates expansion and execution.
 * PARAMS: @shell - shell state, @cmd - command AST node (freed after execution)
 * RETURN: 0 on success, 1 on critical error (malloc/fork)
 * FLOW: expand_cmd -> run_cmd -> wait_last_pid -> free_exp_ast
 * MEMORY: Input AST (t_tok words) -> expanded (char** argv) -> freed
 * NOTE: Parent waits for child only if fork occurred; status in last_status  */
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
