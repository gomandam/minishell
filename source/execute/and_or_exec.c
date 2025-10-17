/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   and_or_exec.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:51:12 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 00:52:17 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * FILE: and_or_exec.c
 * ============================================================================
 * PURPOSE: Logical operators executor - implements short-circuit evaluation
 *          for AND (&&) and OR (||) operators.
 *
 * CONTROL FLOW:
 *   execute_ast_and / execute_ast_or
 *     ├─> execute_ast(left)  [execute.c - recursive]
 *     └─> [conditional] execute_ast(right)
 *
 * LOGIC:
 *   AND (&&): Execute right ONLY if left succeeds (status == 0)
 *   OR  (||): Execute right ONLY if left fails (status != 0)
 *
 * EXTERNAL USAGE: execute_ast [execute.c]
 * NOTE: Both operators free their AST nodes after execution
 * ========================================================================== */

#include "minishell.h"

/* FUNCTION: execute_ast_and
 * PURPOSE: Execute AND operator with short-circuit evaluation.
 * PARAMS: @shell - shell state, @and - AND operator AST node
 * RETURN: 0 on success, 1 on critical error
 * BEHAVIOR: Execute left; if success (status==0), execute right
 * NOTE: Final status is right's status if executed, else left's status  */
int	execute_ast_and(t_shell *shell, t_ast **and)
{
	if (execute_ast(shell, &(*and)->u_data.op.left) == 1 && shell->finished)
		return (free_parse_ast(and), 1);
	if (shell->last_status == 0)
	{
		if (execute_ast(shell, &(*and)->u_data.op.right) == 1
			&& shell->finished)
			return (free_parse_ast(and), 1);
	}
	return (free_parse_ast(and), 0);
}

/* FUNCTION: execute_ast_or
 * PURPOSE: Execute OR operator with short-circuit evaluation.
 * PARAMS: @shell - shell state, @or - OR operator AST node
 * RETURN: 0 on success, 1 on critical error
 * BEHAVIOR: Execute left; if failure (status!=0), execute right
 * NOTE: Final status is right's status if executed, else left's status  */
int	execute_ast_or(t_shell *shell, t_ast **or)
{
	if (execute_ast(shell, &(*or)->u_data.op.left) == 1 && shell->finished)
		return (free_parse_ast(or), 1);
	if (shell->last_status != 0)
	{
		if (execute_ast(shell, &(*or)->u_data.op.right) == 1 && shell->finished)
			return (free_parse_ast(or), 1);
	}
	return (free_parse_ast(or), 0);
}
