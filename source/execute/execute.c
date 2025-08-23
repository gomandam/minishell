/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/22 04:37:12 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// MAIN EXEC. FUNCTION -> Prototype: pipe, execution, process control, 
// 	recursive functions to traverse AST

#include "../../include/minishell.h"

/*
 * Recursively executes the AST node.
 * Deals with the logic based on node type:
 * - AST_CMD:     Executes a command
 * - AST_PIPE:    Handles a pipeline (left | right)
 * - AST_AND_IF:  Handles '&&' logic (executes right only if left succeeds)
 * - AST_OR_IF:   Handles '||' logic (executes right only if left fails)
 * - AST_SUBSH:   Handles a subshell (parentheses)

 * Returns: exit status of the executed subtree
*/

int	execute_ast(t_ast *node, t_env_list *env_list)
{
	if (!node)
		return (0);
	if (node->type == AST_CMD)
		return (expand_ast_cmd(shell, &node->u_data.cmd));
		// Execute Command > Implement cmd execution for built-ins & external
		// Basecase, where to return pid during recursion
	else if (node->type == AST_PIPE)
	{
	// TO DO: pipeline handling
	// setup pipe(), left dup2 write,  right  dup2 read, close fd  wait child process
	// exec_pipe(node); {{ execute_ast(node->left);  execute_ast(node->right); } waitpid(); }
	}
	else if (node->type == AST_AND_IF)
	{
		// TO DO: execute left, if status == 0, execute right
	}
	else if (node->type == AST_OR_IF)
	{
		// TO DO: execute left, if status != 0, execute right
	}
	else if (node->type == AST_SUBSH)		// NOT mandatory or mentioned in evaluation
	{
		// if (expand_subsh(shell, &node->u_data.subsh) != 0)
		//	return (1);
		// TO DO: subshell execution (fork, exec in child process)
	}
	//  Handle errors, unexpected node type
	return (1);
}
	// Think for basecase where the recursion ends
