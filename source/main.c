/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 00:00:07 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/10 14:48:56 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char *argv[], char *envp[])
{
	char	*line;
	t_tok	*tokens;
	t_ast	*ast;

	ft_putstr_fd("Welcome to MiniYeska!\n", 1);
	(void)argc;
	(void)argv;
	(void)envp;
	while (1)
	{
		// TODO: Handle signals, and exit errors for return 1
		line = readline(MINI_PROMPT);
		if (!line)
			break ;
		if (*line)
			add_history(line);
		if (tokenize(line, &tokens) == 1)
		{
			ft_free((void **)&line);
			break ;
		}
		if (tokens == NULL)
		{
			ft_free((void **)&line);
			continue ;
		}
		debug_tokenizer(tokens);
		if (parse_ast(tokens, &ast) == 1)
		{
			// free_tokens(&tokens);
			ft_free((void **)&line);
			continue ;
		}
		debug_parser(ast);
		free_ast_parse(&ast); // ? Must use free_ast_final when ast is expanded
		// free_tokens(&tokens); // TODO: free tokens must not free because are freed
		ft_free((void **)&line);
	}
	rl_clear_history();
}
