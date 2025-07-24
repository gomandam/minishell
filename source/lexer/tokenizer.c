/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 03:41:41 by migugar2          #+#    #+#             */
/*   Updated: 2025/07/24 20:33:16 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// lexer cursor

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

// create tok/seg

t_tok	*new_tok(t_toktype type)
{
	t_tok	*tok;

	tok = malloc(sizeof(t_tok));
	if (!tok)
		return (NULL);
	tok->type = type;
	tok->slice.begin = NULL;
	tok->slice.len = 0;
	tok->seg_head = NULL;
	tok->seg_tail = NULL;
	tok->next = NULL;
	return (tok);
}

t_seg	*new_seg(t_segtype k, const char *ptr, size_t len)
{
	t_seg	*seg;

	seg = malloc(sizeof(t_seg));
	if (!seg)
		return (NULL);
	seg->type = k;
	seg->slice.begin = ptr;
	seg->slice.len = len;
	seg->next = NULL;
	return (seg);
}

void	tok_push(t_tok **head, t_tok **tail, t_tok *tok)
{
	if (!tok)
		return ;
	if (*head == NULL)
		*head = tok;
	else
		(*tail)->next = tok;
	*tail = tok;
}

// tok construct helpers

int	start_word(t_lexer *lx)
{
	if (lx->tok && lx->tok->type == T_WORD)
		return (0); // Unfinished word
	lx->tok = new_tok(T_WORD);
	if (!lx->tok)
		return (1);
	return (0);
}

int	add_seg(t_lexer *lx, t_segtype type, const char *ptr, size_t len)
{
	t_seg	*seg;

	seg = new_seg(type, ptr, len);
	if (!seg)
		return (1);
	if (!lx->tok->seg_head)
		lx->tok->seg_head = seg;
	else
		lx->tok->seg_tail->next = seg;
	lx->tok->seg_tail = seg;
	return (0);
}

void	emit_tok(t_lexer *lx)
{
	if (!lx->tok)
		return ;
	tok_push(&lx->head, &lx->tail, lx->tok);
	lx->tok = NULL;
}

t_lxstate	emit_op(t_lexer *lx, t_toktype type, size_t len)
{
	t_tok	*operator;

	emit_tok(lx);
	operator = new_tok(type);
	if (!operator)
		return (LX_ERR);
	operator->slice.begin = lx->cur;
	operator->slice.len = len;
	tok_push(&lx->head, &lx->tail, operator);
	lx_advance_n(lx, len);
	return (LX_GENERAL);
}

int	ft_iscutword(char c)
{
	if (ft_isspace(c)
		|| c == '|' || c == '&' || c == '<' || c == '>' || c == '('
		|| c == ')' || c == '\'' || c == '"' || c == '$' || c == '*')
		return (1);
	return (0);
}

t_lxstate	handle_general(t_lexer *lx)
{
	if (ft_isspace(*lx->cur)) // TODO: Check if must be here
	{
		emit_tok(lx);
		while (ft_isspace(*lx->cur))
			lx_advance(lx);
	}
	if (*lx->cur == '\0')
		return (LX_EOF);
	if (*lx->cur == '|' && lx->cur[1] == '|')
		return (emit_op(lx, T_OR_IF, 2));
	if (*lx->cur == '&' && lx->cur[1] == '&')
		return (emit_op(lx, T_AND_IF, 2));
	if (*lx->cur == '<' && lx->cur[1] == '<')
		return (emit_op(lx, T_HEREDOC, 2));
	if (*lx->cur == '>' && lx->cur[1] == '>')
		return (emit_op(lx, T_APPEND, 2));
	if (*lx->cur == '|')
		return (emit_op(lx, T_PIPE, 1));
	if (*lx->cur == '<')
		return (emit_op(lx, T_INFILE, 1));
	if (*lx->cur == '>')
		return (emit_op(lx, T_OUTFILE, 1));
	if (*lx->cur == '(')
		return (emit_op(lx, T_LPAREN, 1));
	if (*lx->cur == ')')
		return (emit_op(lx, T_RPAREN, 1));
	if (*lx->cur == '\'')
		return (lx_advance(lx), LX_IN_SINGLE_Q);
	if (*lx->cur == '"')
		return (lx_advance(lx), LX_IN_DOUBLE_Q);
	if (*lx->cur == '$' && (lx->cur[1] == '?' || lx->cur[1] == '_' || ft_isalnum(lx->cur[1])))
	{
		if (start_word(lx) == 1)
			return (LX_ERR);
		lx->prev = lx->state;
		lx_advance(lx);
		return (LX_PARAM);
	}
	if (*lx->cur == '*')
	{
		if (start_word(lx) == 1)
			return (LX_ERR);
		if (add_seg(lx, SEG_WILDCARD, lx->cur, 1) == 1)
			return (ft_free((void **)&lx->tok), LX_ERR);
		lx_advance(lx);
		return (LX_GENERAL);
	}
	const char *start;

	start = lx->cur;
	if (start_word(lx) == 1)
		return (LX_ERR);
	while (*lx->cur && !ft_iscutword(*lx->cur)) // TODO: Manage bug when a '$' or '&' have symbols after
		lx_advance(lx);
	if (add_seg(lx, SEG_TEXT, start, lx->cur - start) == 1)
		return (ft_free((void **)&lx->tok), LX_ERR); // TODO: Free functions
	return (LX_GENERAL);
}

t_lxstate	handle_in_single_q(t_lexer *lx)
{
	const char	*start;

	start = lx->cur;
	while (*lx->cur && *lx->cur != '\'')
		lx_advance(lx);
	if (*lx->cur == '\0')
		return (LX_DIE);
	if (start_word(lx) == 1)
		return (LX_ERR);
	if (add_seg(lx, SEG_TEXT, start, lx->cur - start) == 1)
		return (ft_free((void **)&lx->tok), LX_ERR); // TODO: Create a function for free tok, and another for free seg
	lx_advance(lx);
	return (LX_GENERAL);
}

t_lxstate	handle_in_double_q(t_lexer *lx)
{
	const char	*start;

	start = lx->cur;
	while (*lx->cur && *lx->cur != '\"')
	{
		if (*lx->cur == '$' && (lx->cur[1] == '?' || lx->cur[1] == '_' || ft_isalnum(lx->cur[1])))
			break;
		lx_advance(lx);
	}
	if (*lx->cur == '\0')
		return (LX_DIE);
	if (start_word(lx) == 1)
		return (LX_ERR);
	if (add_seg(lx, SEG_TEXT, start, lx->cur - start) == 1)
		return (ft_free((void **)&lx->tok), LX_ERR); // TODO: Free functions
	if (*lx->cur == '$')
	{
		// * Probably can create a function, return the new state and preserve the prev state
		// * also, check if can we save all the prev state (in a stack), or do it in the parser
		lx_advance(lx);
		lx->prev = lx->state;
		return (LX_PARAM);
	}
	lx_advance(lx);
	return (LX_GENERAL);
}

t_lxstate	handle_param(t_lexer *lx)
{
	const char	*start;

	start = lx->cur;
	if (*lx->cur == '?')
		lx_advance(lx);
	else
	{
		while (*lx->cur && (ft_isalnum(*lx->cur) || *lx->cur == '_'))
			lx_advance(lx);
	}
	if (add_seg(lx, SEG_PARAM, start, lx->cur - start) == 1)
		return (ft_free((void **)&lx->tok), LX_ERR);
	return (lx->prev);
}

t_lxstate	handle_finish(t_lexer *lx)
{
	return (lx->state);
}

t_lxhandler	lexer_handler(t_lxstate state)
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

// !
void	print_tokens(t_tok *head)
{
	t_tok	*tok;
	t_seg	*seg;

	tok = head;
	while (tok)
	{
		printf("Token type: %d, slice: [%.*s]\n", tok->type, (int)tok->slice.len, tok->slice.begin);
		seg = tok->seg_head;
		while (seg)
		{
			printf("  Segment type: %d, slice: [%.*s]\n", seg->type, (int)seg->slice.len, seg->slice.begin);
			seg = seg->next;
		}
		tok = tok->next;
	}
}

int	tokenize(const char *input, t_tok **out)
{
	t_lexer	lx;

	lx.tok = NULL;
	lx.head = NULL;
	lx.tail = NULL;
	lx.input = input;
	lx.cur = input;
	lx.state = LX_GENERAL;
	while (lx.state != LX_EOF && lx.state != LX_DIE && lx.state != LX_ERR)
	{
		lx.state = lexer_handler(lx.state)(&lx);
	}
	emit_tok(&lx);
	print_tokens(lx.head); // !
	if (lx.state == LX_ERR || lx.state == LX_DIE)
	{
		*out = NULL;
		// TODO: Free allocated tokens and segments
	}
	if (lx.state == LX_ERR)
		return (1);
	*out = lx.head;
	return (0);
}
