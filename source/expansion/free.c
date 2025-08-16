/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:50:24 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/16 17:35:51 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_param(t_param **param, t_seg *from_seg)
{
	if (param == NULL || *param == NULL)
		return ;
	if (from_seg != NULL
		&& from_seg->slice.len == 1 && from_seg->slice.begin[0] == '?')
		free((char *)(*param)->value);
	free(*param);
	*param = NULL;
}

void	free_paramlst(t_param **head, t_param **tail, t_seg *segs)
{
	t_seg	*current_seg;
	t_param	*next;

	if (head == NULL || *head == NULL)
		return ;
	current_seg = segs;
	while (current_seg != NULL)
	{
		if (current_seg->type == SEG_PARAM)
			free_param(head, current_seg);
		current_seg = current_seg->next;
	}
	while (*head != NULL)
	{
		next = (*head)->next;
		free_param(head, NULL);
		*head = next;
	}
	*tail = NULL;
}

void	free_argv(t_argv *argv)
{
	if (argv == NULL)
		return ;
	ft_lstclear(&argv->head, free);
	argv->tail = NULL;
}

void	free_exp_redir(t_redir **redir)
{
	if (redir == NULL || *redir == NULL)
		return ;
	if ((*redir)->type != R_HEREDOC)
	{
		free((*redir)->u_data.name);
		(*redir)->u_data.name = NULL;
	}
	else
	{
		// TODO: close pipefd[0];
	}
	free(*redir);
	*redir = NULL;
}

void	free_exp_redirslst(t_redir **head)
{
	t_redir	*next;

	if (head == NULL || *head == NULL)
		return ;
	while (*head != NULL)
	{
		next = (*head)->next;
		free_exp_redir(head);
		*head = next;
	}
}

void	free_exp_redirs(t_redirs *redirs)
{
	if (redirs == NULL)
		return ;
	free_redirslst(&redirs->head);
	redirs->tail = NULL;
}
