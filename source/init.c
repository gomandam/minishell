/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 17:49:59 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/15 18:07:37 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	init_shell(t_shell *shell, char *envp[])
{
	(void)envp;
	shell->line = NULL;
	shell->tokens = NULL;
	shell->ast = NULL;
 	shell->last_status = 0; 
	shell->env_list.head = NULL;
	shell->env_list.tail = NULL;
	shell->env_list.envp = NULL;
	shell->env_list.size = 0;
	return (0);
}
