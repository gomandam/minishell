/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/17 23:45:52 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Recursive function that executes the AST node
 * Deals with the logic based on node type:
 * - AST_CMD:     Executes a command
 * - AST_PIPE:    Handles a pipeline (left | right)
 * - AST_AND_IF:  Handles '&&' logic (executes right only if left succeeds)
 * - AST_OR_IF:   Handles '||' logic (executes right only if left fails)
 * - AST_SUBSH:   Handles a subshell (parentheses)
 * @return status of the executed subtree */
int	execute_ast(t_shell *shell, t_ast **node)
{
	if (!node)
		return (0);
	if ((*node)->type == AST_CMD)
		return (execute_ast_cmd(shell, node));
	else if ((*node)->type == AST_PIPE)
		return (execute_ast_pipe(shell, node));
	else if ((*node)->type == AST_AND_IF)
		return (execute_ast_and(shell, node));
	else if ((*node)->type == AST_OR_IF)
		return (execute_ast_or(shell, node));
	else if ((*node)->type == AST_SUBSH)
		return (execute_ast_subsh(shell, node));
	return (1);
}

/*
 * ============================================================================
 * FILE: execute.c
 * ============================================================================
 *
 * PURPOSE:
 *   Main execution orchestrator for the minishell AST (Abstract Syntax Tree).
 *   Acts as the entry point and dispatcher for all execution operations.
 *
 * CONTROL FLOW TREE:
 *
 *   execute_ast (MAIN DISPATCHER)
 *        |
 *        +-- AST_CMD -----> execute_ast_cmd()     [cmd_exec.c]
 *        |                      |
 *        |                      +-> expand_cmd()  [expansion/command.c]
 *        |                      +-> run_cmd()     [cmd_exec.c]
 *        |                           |
 *        |                           +-> get_builtin()
 *        |                           +-> run_builtin() [builtin_exec.c]
 *        |                           +-> run_external() [cmd_exec.c]
 *        |
 *        +-- AST_PIPE ----> execute_ast_pipe()    [pipe_exec.c]
 *        |                      |
 *        |                      +-> execute_seq_pipe()
 *        |                      +-> wait_last_pid()
 *        |
 *        +-- AST_AND_IF --> execute_ast_and()     [and_or_exec.c]
 *        |                      |
 *        |                      +-> execute_ast() (RECURSIVE)
 *        |
 *        +-- AST_OR_IF ---> execute_ast_or()      [and_or_exec.c]
 *        |                      |
 *        |                      +-> execute_ast() (RECURSIVE)
 *        |
 *        +-- AST_SUBSH ---> execute_ast_subsh()   [subsh_exec.c]
 *                               |
 *                               +-> fork()
 *                               +-> run_subsh()
 *
 * RELATIONSHIPS:
 *   - Called by: repl_evaluate() [source/repl.c]
 *   - Calls to: All execution modules (cmd, pipe, and_or, subsh)
 *   - Memory: Frees AST nodes after execution via free_parse_ast()
 *
 * MODULE INTEGRATION:
 *   - Entry point for execution phase after parsing
 *   - Coordinates with expansion module for parameter/wildcard expansion
 *   - Interfaces with builtin and external command executors
 *   - Manages process creation and synchronization for pipes/subshells
 *
 * NOTES:
 *   - Recursive function: handles nested structures (and, or, pipes)
 *   - AST nodes are freed progressively during execution
 *   - Returns 1 on critical error (malloc, fork failures)
 *   - Returns 0 on successful execution (check shell->last_status for exit)
 * ============================================================================
 */
