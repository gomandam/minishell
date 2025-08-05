/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 21:46:00 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/03 19:00:06 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	collect_redirs(t_parser *parser, t_redirs *redirs)
{
	while (parser->cur && is_redirtok(parser->cur))
	{
		if (collect_redir(parser, redirs) == 1)
			return (1);
	}
	return (0);
}

t_ast	*new_cmd_leaf(void)
{
	t_ast	*ast;

	ast = malloc(sizeof(t_ast));
	if (ast == NULL)
		return (NULL);
	ast->type = AST_CMD;
	ast->u_data.cmd.u_data.words = NULL;
	ast->u_data.cmd.count = 0;
	ast->u_data.cmd.redir.head = NULL;
	ast->u_data.cmd.redir.tail = NULL;
	return (ast);
}

static int	parse_cmd(t_parser *parser, t_ast **out)
{
	t_ast	*cmd;
	t_tok	*last_word;
	t_tok	*tok;

	cmd = new_cmd_leaf();
	if (cmd == NULL)
	{
		parser->err = PARSER_ERR_MEMORY;
		return (1);
	}
	if (collect_redirs(parser, &cmd->u_data.cmd.redir) == 1)
	{
		free_ast_parse(&cmd);
		return (1);
	}
	if (parser->cur == NULL)
	{
		free_ast_parse(&cmd);
		parser->err = PARSER_ERR_UNEXPECTED_EOF;
		return (1);
	}
	if (parser->cur->type != T_WORD)
	{
		free_ast_parse(&cmd);
		parser->err = PARSER_ERR_SYNTAX;
		return (1);
	}
	last_word = NULL;
	while (parser->cur && (parser->cur->type == T_WORD
			|| is_redirtok(parser->cur)))
	{
		if (parser->cur->type == T_WORD)
		{
			tok = parser->cur;
			parser->cur = parser->cur->next;
			tok->next = NULL;
			if (cmd->u_data.cmd.u_data.words == NULL)
				cmd->u_data.cmd.u_data.words = tok;
			else
				last_word->next = tok;
			last_word = tok;
			cmd->u_data.cmd.count++;
		}
		else if (collect_redir(parser, &cmd->u_data.cmd.redir) == 1)
		{
			free_ast_parse(&cmd);
			return (1);
		}
	}
	*out = cmd;
	return (0);
}

t_ast	*new_subsh_node(t_ast *child)
{
	t_ast	*ast;

	ast = malloc(sizeof(t_ast));
	if (ast == NULL)
		return (NULL);
	ast->type = AST_SUBSH;
	ast->u_data.subsh.child = child;
	ast->u_data.subsh.redir.head = NULL;
	ast->u_data.subsh.redir.tail = NULL;
	return (ast);
}

static int	parse_subsh(t_parser *parser, t_ast **out)
{
	t_ast	*child;
	t_tok	*next;

	next = parser->cur->next;
	free_tok(&parser->cur);
	parser->cur = next;
	if (parse_and_or(parser, &child) == 1)
		return (1);
	if (parser->cur == NULL)
	{
		parser->err = PARSER_ERR_UNEXPECTED_EOF;
		return (1);
	}
	if (parser->cur->type != T_RPAREN)
	{
		parser->err = PARSER_ERR_SYNTAX;
		return (1);
	}
	next = parser->cur->next;
	free_tok(&parser->cur);
	parser->cur = next;
	child = new_subsh_node(child);
	if (child == NULL)
	{
		parser->err = PARSER_ERR_MEMORY;
		return (1);
	}
	if (collect_redirs(parser, &child->u_data.subsh.redir) == 1)
	{
		free_ast_parse(&child);
		return (1);
	}
	*out = child;
	return (0);
}

int	parse_cmd_subsh(t_parser *parser, t_ast **out)
{
	if (parser->cur->type == T_LPAREN)
		return (parse_subsh(parser, out));
	return (parse_cmd(parser, out));
}
