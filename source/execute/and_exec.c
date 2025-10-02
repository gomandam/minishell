/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   and_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 01:51:38 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/02 01:52:48 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_ast_and(t_shell *shell, t_ast **node)
{
	printf("implement and logic.");
	free_parse_ast(node);
	return (0);
	/* storage = execute_ast(shell, &(*node)->u_data.op.left);
	if (storage == 0)
		return (execute_ast(shell, &(*node)->u_data.op.right));
	return (storage); */
}
