/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 00:00:07 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/02 23:18:17 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char *argv[], char *envp[])
{
	t_shell	shell;

	(void)argv;
	shell.last_status = 0;
	if (argc != 1)
		perror_usage(&shell);
	else
	{
		if (init_shell(&shell, envp) == 0)
			repl(&shell);
	}
	return (shell.last_status);
}
