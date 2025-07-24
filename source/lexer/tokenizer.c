/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 03:41:41 by migugar2          #+#    #+#             */
/*   Updated: 2025/07/24 14:15:53 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef enum e_toktype
{
	T_WORD,
	T_PIPE,
	T_AND_IF,
	T_OR_IF,
	T_INFILE,
	T_HEREDOC,
	T_OUTFILE,
	T_APPEND,
	T_LPAREN,
	T_RPAREN,
}	t_toktype;

typedef enum e_segtype
{
	SEG_TEXT,
	SEG_PARAM,
	SEG_WILDCARD
}	t_segtype;

typedef enum e_lxstate
{
	LX_GENERAL,
	LX_IN_SINGLE_Q,
	LX_IN_DOUBLE_Q,
	LX_PARAM,
	LX_EOF,
	LX_DIE, // Unvalid state, without error
	LX_ERR // Malloc error
}	t_lxstate;

typedef struct s_slice
{
	const char	*begin;
	size_t		len;
}	t_slice;

typedef struct s_seg
{
	t_segtype		type;
	t_slice			slice;
	struct s_seg	*next;
}	t_seg;

typedef struct s_tok
{
	t_toktype		type;
	t_slice			slice;
	t_seg			*seg_head;
	t_seg			*seg_tail;
	struct s_tok	*next;
}	t_tok;

typedef struct s_lexer
{
	const char		*input;
	const char		*cur;
	t_lxstate		prev;
	t_lxstate		state;
	t_tok			*tok;
	t_tok			*head;
	t_tok			*tail;
}	t_lexer;

typedef t_lxstate	(*t_lxhandler)(t_lexer *lx);

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
		return (0);
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

int	emit_op(t_lexer *lx, t_toktype type, size_t len)
{
	t_tok	*operator;

	emit_tok(lx);
	operator = new_tok(type);
	if (!operator)
		return (1);
	operator->slice.begin = lx->cur;
	operator->slice.len = len;
	tok_push(&lx->head, &lx->tail, operator);
	lx_advance_n(lx, len);
	return (0);
}

t_lxstate	handle_general(t_lexer *lx)
{
	return (lx->state);
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
	while (*lx->cur && *lx->cur != '\"' && *lx->cur != '$')
		lx_advance(lx);
	if (*lx->cur == '\0')
		return (LX_DIE);
	if (start_word(lx) == 1)
		return (LX_ERR);
	if (add_seg(lx, SEG_TEXT, start, lx->cur - start) == 1)
		return (ft_free((void **)&lx->tok), LX_ERR); // TODO: Free functions
	if (*lx->cur == '$') // TODO: It's not checking if is only '$'
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
	return (lx->state);
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
	if (lx.state == LX_ERR)
	{
		// TODO: Free allocated tokens and segments
	}
	return (0);
}
