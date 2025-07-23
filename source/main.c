/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 00:00:07 by migugar2          #+#    #+#             */
/*   Updated: 2025/07/23 05:18:41 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char *argv[], char *envp[])
{
	char	*line;

	ft_putstr_fd("Welcome to MiniYeska!\n", 1);
	(void)argc;
	(void)argv;
	(void)envp;
	while (1)
	{
		// TODO: Handle signals
		line = readline(MINI_PROMPT);
		if (!line)
			break ;
		if (*line)
			add_history(line);
		printf("You entered: %s\n", line);
		ft_free((void **)&line);
	}
	rl_clear_history();
}
