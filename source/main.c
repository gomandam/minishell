/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 00:00:07 by migugar2          #+#    #+#             */
/*   Updated: 2025/07/24 22:51:08 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// ! Temporal
void	print_tokens(t_tok *head)
{
	t_tok	*tok;
	t_seg	*seg;

	tok = head;
	while (tok)
	{
		printf("Token type: %d, slice: [%.*s]\n", tok->type, (int)tok->slice.len, tok->slice.begin);
		seg = tok->seg_head;
		while (seg)
		{
			printf("  Segment type: %d, slice: [%.*s]\n", seg->type, (int)seg->slice.len, seg->slice.begin);
			seg = seg->next;
		}
		tok = tok->next;
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	char	*line;
	t_tok	*tokens;

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
		if (tokenize(line, &tokens) == 1)
		{
			ft_free((void **)&line);
			break ;
		}
		print_tokens(tokens);
		free_tokens(&tokens);
		ft_free((void **)&line);
	}
	rl_clear_history();
}
