/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:22:14 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/07 17:50:44 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirtok(t_tok *tok)
{
	if (tok == NULL)
		return (0);
	return (tok->type == T_INFILE || tok->type == T_OUTFILE
		|| tok->type == T_HEREDOC || tok->type == T_APPEND);
}

void	consume_tok(t_parser *parser)
{
	t_tok	*next;

	if (parser->cur == NULL)
		return ;
	next = parser->cur->next;
	free_tok(&parser->cur);
	parser->cur = next;
}

char	*get_text_tok(t_tok *tok)
{
	if (tok->type == T_AND_IF)
		return ("&&");
	else if (tok->type == T_OR_IF)
		return ("||");
	else if (tok->type == T_PIPE)
		return ("|");
	else if (tok->type == T_LPAREN)
		return ("(");
	else if (tok->type == T_RPAREN)
		return (")");
	else if (tok->type == T_INFILE)
		return ("<");
	else if (tok->type == T_OUTFILE)
		return (">");
	else if (tok->type == T_HEREDOC)
		return ("<<");
	else if (tok->type == T_APPEND)
		return (">>");
	return ("");
}
