/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 21:46:59 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/08 16:23:13 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_redir	*new_redir(t_redirtype type, t_tok *word)
{
	t_redir		*redir;

	redir = malloc(sizeof(t_redir));
	if (redir == NULL)
		return (NULL);
	redir->type = type;
	redir->u_data.word = word;
	redir->next = NULL;
	return (redir);
}

t_redir	*new_redir_from_tok(t_tok *op, t_tok *word)
{
	t_redirtype	type;

	if (op->type == T_INFILE)
		type = R_INFILE;
	if (op->type == T_INFILE)
		type = R_INFILE;
	else if (op->type == T_HEREDOC)
		type = R_HEREDOC;
	else if (op->type == T_OUTFILE)
		type = R_OUTFILE;
	else if (op->type == T_APPEND)
		type = R_APPEND;
	else
		return (NULL);
	return (new_redir(type, word));
}

void	redir_push(t_redirs *list, t_redir *redir)
{
	if (!redir)
		return ;
	if (list->head == NULL)
		list->head = redir;
	else
		list->tail->next = redir;
	list->tail = redir;
}

int	collect_redir(t_parser *parser, t_redirs *list)
{
	t_redir	*redir;
	t_tok	*op;
	t_tok	*word;

	op = parser->cur;
	word = op->next;
	if (word == NULL)
		return (printerr_unexpecteol());
	if (word->type != T_WORD)
		return (printerr_syntaxtok(word));
	redir = new_redir_from_tok(op, word);
	if (redir == NULL)
		return (printerr_malloc());
	redir_push(list, redir);
	parser->cur = word->next;
	free_tok(&op);
	return (0);
}

int	collect_redirs(t_parser *parser, t_redirs *redirs)
{
	while (parser->cur && is_redirtok(parser->cur))
	{
		if (collect_redir(parser, redirs) == 1)
			return (1);
	}
	return (0);
}
