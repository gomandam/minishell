/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 23:00:52 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/25 20:28:17 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_tok(t_exp *exp, t_tok *tok)
{
	t_seg	*cur;
	size_t	i;
	char	*expanded;

	expanded = malloc(sizeof(char) * (exp->len + 1));
	if (expanded == NULL)
		return (perror_malloc(), NULL);
	i = 0;
	cur = tok->seg_head;
	while (cur != NULL)
	{
		if (cur->type == SEG_TEXT)
		{
			ft_strlcpy(&expanded[i], cur->slice.begin, cur->slice.len + 1);
			i += cur->slice.len;
		}
		else if (cur->type == SEG_WILDCARD)
			expanded[i++] = '*';
		else if (cur->type == SEG_PARAM)
			i += consume_param(exp, cur, &expanded[i]);
		cur = cur->next;
	}
	expanded[i] = '\0';
	return (expanded);
}

static void	count_cur(t_exp	*exp, t_seg *cur)
{
	if (cur->type == SEG_TEXT)
		exp->words += 1;
	else if (cur->type == SEG_PARAM)
	{
		exp->paramc += 1;
		if (exp->tail != NULL && exp->tail->value == NULL)
			exp->paramnull += 1;
	}
	else if (cur->type == SEG_WILDCARD)
		exp->wildcards += 1;
}

static int	get_info(t_shell *shell, t_exp *exp, t_tok *tok)
{
	t_seg	*cur;
	t_param	*param;

	cur = tok->seg_head;
	while (cur != NULL)
	{
		if (cur->type == SEG_TEXT)
			exp->len += cur->slice.len;
		else if (cur->type == SEG_PARAM)
		{
			param = expand_param(shell, cur);
			if (param == NULL)
				return (perror_malloc(),
					free_paramlst(&exp->head, &exp->tail, tok->seg_head), 1);
			param_push(&exp->head, &exp->tail, param);
			exp->len += param->len;
		}
		else if (cur->type == SEG_WILDCARD)
			exp->len++;
		count_cur(exp, cur);
		cur = cur->next;
	}
	return (0);
}

int	expansion(t_shell *shell, t_tok *tok, t_argv *argv)
{
	t_exp	exp;
	char	*expanded;

	exp.head = NULL;
	exp.tail = NULL;
	exp.len = 0;
	exp.paramc = 0;
	exp.paramnull = 0;
	exp.words = 0;
	exp.wildcards = 0;
	if (get_info(shell, &exp, tok) == 1)
		return (1);
	if (exp.paramc > 0 && exp.paramnull == exp.paramc && exp.words == 0)
		return (free_paramlst(&exp.head, &exp.tail, tok->seg_head), 0);
	expanded = expand_tok(&exp, tok);
	if (expanded == NULL)
		return (free_paramlst(&exp.head, &exp.tail, tok->seg_head), 1);
	if (exp.wildcards == 0)
	{
		if (new_argv_push(argv, expanded) == 1)
			return (free(expanded), 1);
	}
	else if (expand_wildcards(shell, expanded, argv) == 1)
		return (1);
	return (0);
}
