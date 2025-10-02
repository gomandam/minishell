/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:22:22 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/01 12:12:27 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_redir(t_redir **redir, int close_fd)
{
	if (!redir || !*redir)
		return ;
	free_tok(&(*redir)->u_data.word);
	if (close_fd)
		ft_close(&(*redir)->fd);
	free(*redir);
	*redir = NULL;
}

void	free_redirslst(t_redir **head)
{
	t_redir	*next;

	if (!head || !*head)
		return ;
	while (*head != NULL)
	{
		next = (*head)->next;
		free_redir(head, 1);
		*head = next;
	}
	*head = NULL;
}

void	free_redirs(t_redirs *redirs)
{
	if (!redirs)
		return ;
	free_redirslst(&redirs->head);
	redirs->head = NULL;
	redirs->tail = NULL;
}

void	free_parse_ast_cmd(t_ast **ast)
{
	if (!ast || !*ast)
		return ;
	if ((*ast)->u_data.cmd.u_data.words != NULL)
		free_tokens(&(*ast)->u_data.cmd.u_data.words);
	free_redirs(&(*ast)->u_data.cmd.redir);
	free(*ast);
	*ast = NULL;
}

void	free_parse_ast(t_ast **ast)
{
	if (!ast || !*ast)
		return ;
	if ((*ast)->type == AST_PIPE || (*ast)->type == AST_AND_IF
		|| (*ast)->type == AST_OR_IF)
	{
		free_parse_ast(&(*ast)->u_data.op.left);
		free_parse_ast(&(*ast)->u_data.op.right);
		ft_close(&(*ast)->u_data.op.pipe_fd[0]);
		ft_close(&(*ast)->u_data.op.pipe_fd[1]);
	}
	else if ((*ast)->type == AST_SUBSH)
	{
		free_parse_ast(&(*ast)->u_data.subsh.child);
		free_redirs(&(*ast)->u_data.subsh.redir);
	}
	else if ((*ast)->type == AST_CMD)
		free_parse_ast_cmd(ast);
	free(*ast);
	*ast = NULL;
}
