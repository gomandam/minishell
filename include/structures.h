/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 02:42:29 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/07 20:40:02 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

# include <sys/types.h> // pid_t, size_t

typedef struct s_slice
{
	const char	*begin;
	size_t		len;
}	t_slice;

typedef enum e_segtype
{
	SEG_TEXT,
	SEG_PARAM,
	SEG_WILDCARD
}	t_segtype;

typedef struct s_seg
{
	t_segtype		type;
	t_slice			slice;
	struct s_seg	*next;
}	t_seg;

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

typedef struct s_tok
{
	t_toktype		type;
	t_seg			*seg_head;
	t_seg			*seg_tail;
	struct s_tok	*next;
}	t_tok;

typedef enum e_redirtype
{
	R_INFILE,
	R_HEREDOC,
	R_OUTFILE,
	R_APPEND
}	t_redirtype;

typedef struct s_redir
{
	union
	{
		t_tok			*word;
		char			*text;
		int16_t			pipe[2]; // TODO: For heredoc
	}			u_data;
	t_redirtype		type;
	struct s_redir	*next;
}	t_redir;

typedef struct s_redirs
{
	t_redir		*head;
	t_redir		*tail;
}	t_redirs;

typedef struct s_cmd
{
	union
	{
		t_tok			*words;
		char			**argv;
	}			u_data;
	size_t			count;
	t_redirs		redir;
}	t_cmd;

typedef struct s_op
{
	struct s_ast	*left;
	struct s_ast	*right;
}	t_op;

typedef struct s_subsh
{
	struct s_ast	*child;
	t_redirs		redir;
}	t_subsh;

typedef enum e_asttype
{
	AST_CMD,
	AST_PIPE,
	AST_AND_IF,
	AST_OR_IF,
	AST_SUBSH
}	t_asttype;

typedef struct s_ast
{
	union
	{
		t_cmd	cmd;
		t_op	op;
		t_subsh	subsh;
	}			u_data;
	t_asttype		type;
}	t_ast;

// lexer

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

// parse

typedef struct s_parser
{
	t_tok		*cur;
}	t_parser;

#endif
