/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   or_exec.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 01:51:54 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/02 02:55:43 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_ast_or(t_shell *shell, t_ast **or)
{
	(void)shell;
	printf("implement or logic.\n");
	free_parse_ast(or);
	return (0);
	/* storage = execute_ast(shell, &(*node)->u_data.op.left);
	if (storage != 0)
		return (execute_ast(shell, &(*node)->u_data.op.right));
	return (storage); */
}
