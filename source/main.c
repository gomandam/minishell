/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 00:00:07 by migugar2          #+#    #+#             */
/*   Updated: 2025/07/26 02:09:54 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// ! Temporal
void	debug_tokens(t_tok *head)
{
	const char *toktype_names[] = {
		"T_WORD", "T_PIPE", "T_AND_IF", "T_OR_IF",
		"T_INFILE", "T_HEREDOC", "T_OUTFILE", "T_APPEND",
		"T_LPAREN", "T_RPAREN"
	};
	const char *segtype_names[] = {
		"SEG_TEXT", "SEG_PARAM", "SEG_WILDCARD"
	};
	printf("-> Tokens:\n");
	t_tok	*tok = head;
	while (tok != NULL)
	{
		printf("	Token type: %s (%d)\n", toktype_names[tok->type], tok->type);
		t_seg	*seg = tok->seg_head;
		while (seg != NULL)
		{
			printf("		Segment type: %s (%d), slice: [%.*s]\n", segtype_names[seg->type], seg->type, (int)seg->slice.len, seg->slice.begin);
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
		debug_tokens(tokens);
		free_tokens(&tokens);
		ft_free((void **)&line);
	}
	rl_clear_history();
}
