/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 02:07:43 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/07 20:22:29 by migugar2         ###   ########.fr       */
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
	if (parser->err == PARSER_ERR_UNEXPECTED_EOF)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `newline`\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

int	parse_ast(t_tok *tok, t_ast **out)
{
	t_parser	parser;

	parser.cur = tok;
	*out = NULL;
	if (parse_and_or(&parser, out) == 1)
		return (free_tokens(&parser.cur), free_ast_parse(out), 1);
	if (parser.cur != NULL)
		return (printerr_syntaxtok(parser.cur), free_tokens(&parser.cur),
			free_ast_cmd_parse(out), 1);
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
