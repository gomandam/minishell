/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subsh_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:53:06 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 00:54:07 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * ============================================================================
 * FILE: subsh_exec.c
 * ============================================================================
 * PURPOSE: Subshell executor - handles parenthesized command groups in
 *          isolated child processes with independent redirections.
 *
 * CONTROL FLOW:
 *   execute_ast_subsh
 *     ├─> expand_subsh (expansion/command.c)
 *     ├─> fork()
 *     ├─> [child] run_subsh
 *     │    ├─> signals_exec
 *     │    ├─> external_redirs
 *     │    ├─> execute_ast (recursive)
 *     │    └─> exit(last_status)
 *     └─> [parent] wait_last_pid
 *
 * EXTERNAL USAGE: execute_ast [execute.c], execute_seq_child [pipe_exec.c]
 * NOTE: Child process fully isolated; does not affect parent shell state
 * ========================================================================== */

#include "minishell.h"

/* FUNCTION: run_subsh
 * PURPOSE: Execute subshell child AST in isolated process.
 * PARAMS: @shell - state, @subsh - subshell node, @pipe_seq - in pipeline flag
 * RETURN: Never returns (calls exit)
 * BEHAVIOR: Setup signals, apply redirections, execute child AST, cleanup, exit
 * NOTE: pipe_seq==1 means called from pipeline (don't free shell->ast)  */
void	run_subsh(t_shell *shell, t_ast **subsh, int pipe_seq)
{
	signals_exec(shell);
	if (external_redirs(shell, &(*subsh)->u_data.subsh.redir) == 0)
		execute_ast(shell, &(*subsh)->u_data.subsh.child);
	free_exp_ast(subsh);
	if (shell->line)
		rl_clear_history();
	ft_freestr(&shell->line);
	free_env_list(&shell->env_list);
	if (!pipe_seq)
	{
		free_parse_ast(&shell->ast);
		exit(shell->last_status);
	}
}

/* FUNCTION: execute_ast_subsh
 * PURPOSE: Subshell execution entry - fork and execute child AST.
 * PARAMS: @shell - shell state, @subsh - subshell AST node
 * RETURN: 0 on success, 1 on error
 * BEHAVIOR: Expand redirections, fork, child calls run_subsh, parent waits
 * NOTE: Parent frees expanded AST after wait; child exit status in last_status  */
int	execute_ast_subsh(t_shell *shell, t_ast **subsh)
{
	pid_t	pid;

	if (expand_subsh(shell, &(*subsh)->u_data.subsh) == 1)
		return (free_parse_ast(subsh), 1);
	pid = fork();
	if (pid == -1)
		return (perror_syscall(shell, "minishell: fork"));
	else if (pid == 0)
		run_subsh(shell, subsh, 0);
	wait_last_pid(shell, pid);
	free_exp_ast(subsh);
	return (0);
}
