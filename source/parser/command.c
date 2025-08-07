/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 21:46:00 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/07 20:04:01 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

	if (parser->cur == NULL)
		return (printerr_unexpecteol());
	if (parser->cur->type != T_WORD && !is_redirtok(parser->cur))
		return (printerr_syntaxtok(parser->cur), 1);
	cmd = new_cmd_leaf();
	if (cmd == NULL)
		return (printerr_malloc());
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
			return (free_ast_parse(&cmd), 1);
	}
	// if (parser->cur != NULL && !is_redirtok(parser->cur))
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

	consume_tok(parser);
	if (parse_and_or(parser, &child) == 1)
		return (1);
	if (parser->cur == NULL)
		return (printerr_unexpecteol(), free_ast_parse(&child), 1);
	if (parser->cur->type != T_RPAREN)
		return (printerr_syntaxtok(parser->cur), free_ast_parse(&child), 1);
	consume_tok(parser);
	child = new_subsh_node(child);
	if (child == NULL)
		return (printerr_malloc(), free_ast_parse(&child), 1);
	if (collect_redirs(parser, &child->u_data.subsh.redir) == 1)
		return (free_ast_parse(&child), 1);
	*out = child;
	return (0);
}

int	parse_cmd_subsh(t_parser *parser, t_ast **out)
{
	if (parser->cur && parser->cur->type == T_LPAREN)
		return (parse_subsh(parser, out));
	return (parse_cmd(parser, out));
}
