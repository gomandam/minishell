/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 21:46:59 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/12 19:03:45 by migugar2         ###   ########.fr       */
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
	redir->fd = -1;
	return (redir);
}

t_redir	*new_redir_from_tok(t_tok *op, t_tok *word)
{
	t_redirtype	type;

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

int	collect_redir(t_shell *shell, t_tok **cur, t_redirs *list)
{
	t_redir	*redir;
	t_tok	*op;
	t_tok	*word;
	t_tok	*next;

	op = *cur;
	word = op->next;
	next = word->next;
	if (word == NULL)
		return (perror_unexpecteol(shell));
	if (word->type != T_WORD)
		return (perror_syntaxtok(shell, word));
	redir = new_redir_from_tok(op, word);
	if (redir == NULL)
		return (perror_malloc(shell));
	free_tok(&op);
	if (redir->type == R_HEREDOC)
	{
		if (heredoc_redir(shell, redir) == 1)
			return (free_redir(&redir), 1);
	}
	redir_push(list, redir);
	*cur = next;
	return (0);
}

int	collect_redirs(t_shell *shell, t_tok **cur, t_redirs *redirs)
{
	while (*cur && is_redirtok(*cur))
	{
		if (collect_redir(shell, cur, redirs) == 1)
			return (1);
	}
	return (0);
}
