/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 21:46:59 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/12 21:25:47 by migugar2         ###   ########.fr       */
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

int	append_redir(t_shell *shell, t_tok **cur, t_tok **rdirs, t_tok **rdirs_last)
{
	if (*rdirs == NULL)
		*rdirs = *cur;
	else
		(*rdirs_last)->next = *cur;
	*rdirs_last = *cur;
	*cur = (*cur)->next;
	if ((*cur) == NULL)
		return (perror_unexpecteol(shell), 1);
	*rdirs_last = *cur;
	*cur = (*cur)->next;
	(*rdirs_last)->next = NULL;
	if ((*rdirs_last)->type != T_WORD)
		return (perror_syntaxtok(shell, *rdirs_last), 1);
	return (0);
}
