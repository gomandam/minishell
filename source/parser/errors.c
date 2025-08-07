/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 15:28:33 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/07 18:12:05 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	printerr_malloc(void)
{
	write(STDERR_FILENO, "minishell: memory allocation error\n", 35);
	return (1);
}

int	printerr_unexpecteol(void)
{
	write(STDERR_FILENO,
		"minishell: syntax error near unexpected token 'newline'\n", 56);
	return (1);
}

int	printerr_syntaxtok(t_tok *cur)
{
	char	*tok_text;
	char	*text;
	size_t	tok_size;
	size_t	text_size;

	if (cur == NULL)
		write(STDERR_FILENO, "minishell: syntax error unexpected token\n", 41);
	else
	{
		tok_text = get_text_tok(cur);
		tok_size = 0;
		while (tok_text[tok_size])
			tok_size++;
		text_size = 47 + tok_size + 2;
		text = malloc(sizeof(char) * text_size);
		if (text == NULL)
			return (printerr_malloc());
		ft_memcpy(text, "minishell: syntax error near unexpected token '", 47);
		ft_memcpy(text + 47, tok_text, tok_size);
		text[47 + tok_size] = '\'';
		text[47 + tok_size + 1] = '\n';
		write(STDERR_FILENO, text, text_size);
		ft_free((void **)&text);
	}
	return (1);
}
