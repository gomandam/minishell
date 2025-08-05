/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 02:07:43 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/06 00:29:53 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// TODO: Probably can print error when occurred, save info about error in parser struct and change return value

int	manage_error(t_parser *parser)
{
	// TODO: Add info about tokens in syntax error, manage freeing memory
	// TODO: Probably can manage error when occurred
	free_tokens(&parser->cur);
	if (parser->err == PARSER_ERR_SYNTAX)
	{
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
		return (0);
	}
	if (parser->err == PARSER_ERR_MEMORY)
	{
		ft_putstr_fd("minishell: memory allocation error\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

int	parse_ast(t_tok *tok, t_ast **out)
{
	t_parser	parser;

	parser.cur = tok;
	parser.err = PARSER_ERR_NONE;
	*out = NULL;
	if (parse_and_or(&parser, out) == 1)
	{
		*out = NULL;
		return (manage_error(&parser));
	}
	if (parser.cur != NULL)
	{
		*out = NULL;
		parser.err = PARSER_ERR_SYNTAX;
		return (manage_error(&parser));
	}
	return (0);
}

/*
echo 123 | grep 123 && (echo 123 | grep 123)
	&&
|				\
v					v
  |					fork
|   \					|
v      v			v
echo  grep			|
							|		\
							v			v
						echo		grep
*/
