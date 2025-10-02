/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 01:55:27 by migugar2         ###   ########.fr       */
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
 * @return status of the executed subtree
 */
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
