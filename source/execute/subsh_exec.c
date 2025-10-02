/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subsh_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 01:52:08 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/02 01:54:18 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_ast_subsh(t_shell *shell, t_ast **node)
{
	printf("implement subshell logic.");
	free_parse_ast(node);
	return (0);
	// TO DO: implement subshell logic
	// if (expand_subsh(shell, &node->u_data.subsh) != 0)
	//	return (1);
	// subshell execution (fork, exec in child process AST, handle redirs)
}
