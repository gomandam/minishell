/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   and_or_exec.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 01:51:38 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/02 12:52:53 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
