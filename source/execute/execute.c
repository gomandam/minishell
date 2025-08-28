/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/28 17:37:14 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Recursive function that executes the AST node
 * Deals with the logic based on node type:
 * - AST_CMD:     Executes a command
 * - AST_PIPE:    Handles a pipeline (left | right)
 * - AST_AND_IF:  Handles '&&' logic (executes right only if left succeeds)
 * - AST_OR_IF:   Handles '||' logic (executes right only if left fails)
 * - AST_SUBSH:   Handles a subshell (parentheses)
 	* Returns: exit status of the executed subtree·		*/

#include "../../include/minishell.h"
#include "../../libft/libft.h"

int	execute_ast(t_ast *node, t_env_list *env_list)
{
	t_shell	shell;
	int	storage;

	shell.env_list = *env_list;
	storage = 0;
	if (!node)
		return (0);
	if (node->type == AST_CMD)
		return (expand_ast_cmd(shell, &node->u_data.cmd));
		// Done: Execute Command > Implement cmd execution for built-ins & external
		// Done: Basecase, where to return pid during recursion
	else if (node->type == AST_PIPE)
		return (exec_ast_pipe(&shell, node));
	// DONE: pipeline handling 
	// setup pipe(), left dup2 write,  right  dup2 read, close fd  wait child process
	// exec_pipe(node); {{ execute_ast(node->left);  execute_ast(node->right); } waitpid(); }
	else if (node->type == AST_AND_IF)
	{
		storage = execute_ast(node->u_data.op.left, env_list);
		if (storage == 0)
			return (execute_ast(node->u_data.op.right, env_list));
		return (storage);
	}
	else if (node->type == AST_OR_IF)
	{
		storage = execute_ast(node->u_data.op.left, env_list);
		if (storage != 0)
			return (execute_ast(node->u_data.op.right, env_list));
		return (storage);
	}
	else if (node->type == AST_SUBSH)
	{
		// TO DO: implement subshell logic
		// if (expand_subsh(shell, &node->u_data.subsh) != 0)
		//	return (1);
		// subshell execution (fork, exec in child process AST, handle redirs)
	}
	ft_putstr_fd("minishell: internal error: unexpected AST node type \n", 2);
	return (1);
}
