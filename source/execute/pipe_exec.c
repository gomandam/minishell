/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/26 01:42:04 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include <sys/wait.h>

// basecase
pid_t	expand_cmd(t_ast node)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		execve(argv);
	}
	return (pid);
}

pid_t	execute_pipe(t_ast nodes)
{
	pid_t	pid1;
	pid_t	pid2;

	pid1 = execute_ast_pipe(nodes->left);
	pid2 = execute_ast_pipe(nodes->right);
	waitpid();
	return ();
}

pid_t	execute_ast_pipe(t_shell shell, t_ast nodes, char *envp[])
{
	if (node == AST_PIPE)
		execute_ast_pipe();
	return (expand_cmd(t_ast nodes));
}
