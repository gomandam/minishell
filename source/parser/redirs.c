/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 21:22:50 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/01 12:13:34 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	collect_redir(t_shell *shell, t_tok **cur, t_redirs *list)
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
	*cur = next;
	if (redir->type == R_HEREDOC)
	{
		if (heredoc_redir(shell, redir) == 1)
			return (free_redir(&redir, 1), 1);
	}
	redir_push(list, redir);
	return (0);
}

static void	rm_unused_in(t_redirs *redirs, t_redir *beflastin, t_redir **lastin)
{
	if (beflastin)
		beflastin->next = (*lastin)->next;
	else
		redirs->head = (*lastin)->next;
	free_redir(lastin, 1);
}

int	collect_redirs(t_shell *shell, t_tok **cur, t_redirs *redirs)
{
	t_redir	*bef;
	t_redir	*lastin;
	t_redir	*beflastin;

	lastin = NULL;
	beflastin = NULL;
	while (*cur && is_redirtok(*cur))
	{
		if (redirs->tail == NULL || redirs->tail->type != R_HEREDOC)
			bef = redirs->tail;
		if (collect_redir(shell, cur, redirs) == 1)
			return (free_tokens(cur), 1);
		if (redirs->tail->type == R_HEREDOC || redirs->tail->type == R_INFILE)
		{
			if (lastin && lastin->type == R_HEREDOC)
				rm_unused_in(redirs, beflastin, &lastin);
			beflastin = bef;
			lastin = redirs->tail;
		}
	}
	return (0);
}
