/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 03:41:41 by migugar2          #+#    #+#             */
/*   Updated: 2025/07/26 02:04:34 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	lx_advance(t_lexer *lx)
{
	if (lx->cur && *lx->cur != '\0')
		lx->cur++;
}

void	lx_advance_n(t_lexer *lx, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && lx->cur && *lx->cur != '\0')
	{
		lx->cur++;
		i++;
	}
}

static t_lxhandler	lexer_handler(t_lxstate state)
{
	if (state == LX_GENERAL)
		return (handle_general);
	else if (state == LX_IN_SINGLE_Q)
		return (handle_in_single_q);
	else if (state == LX_IN_DOUBLE_Q)
		return (handle_in_double_q);
	else if (state == LX_PARAM)
		return (handle_param);
	return (handle_finish);
}

int	tokenize(const char *input, t_tok **out)
{
	t_lexer		lx;
	t_lxstate	next;

	lx.tok = NULL;
	lx.head = NULL;
	lx.tail = NULL;
	lx.input = input;
	lx.cur = input;
	lx.state = LX_GENERAL;
	while (lx.state != LX_EOL && lx.state != LX_DIE && lx.state != LX_ERR)
	{
		next = lexer_handler(lx.state)(&lx);
		lx.prev = lx.state;
		lx.state = next;
	}
	emit_tok(&lx);
	if (lx.state == LX_ERR || lx.state == LX_DIE)
	{
		*out = NULL;
		free_tokens(&lx.head);
	}
	if (lx.state == LX_ERR)
		return (1);
	*out = lx.head;
	return (0);
}
