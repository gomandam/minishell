/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 02:42:29 by migugar2          #+#    #+#             */
/*   Updated: 2025/07/26 02:44:30 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

# include <sys/types.h> // pid_t, size_t

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
	LX_EOL,
	LX_DIE,
	LX_ERR
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

#endif
