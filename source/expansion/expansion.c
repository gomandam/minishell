/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 23:00:52 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/02 00:12:07 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
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

t_param	*expand_param(t_shell *shell, t_seg *seg)
{
	t_param	*res;
	char	*key;

	res = new_param();
	if (res == NULL)
		return (NULL);
	if (seg->slice.len == 1 && seg->slice.begin[0] == '?')
	{
		res->value = ft_itoa(shell->last_status);
		res->len = ft_strlen(res->value);
		return (res);
	}
	key = malloc(sizeof(char) * (seg->slice.len + 1));
	if (key == NULL)
		return (free(res), NULL);
	ft_strlcpy(key, seg->slice.begin, seg->slice.len + 1);
	res->value = get_env_value(&shell->env_list, key);
	free(key);
	if (res->value == NULL)
		res->len = 0;
	else
		res->len = ft_strlen(res->value);
	return (res);
}
*/

int	new_builder(t_expand *build)
{
	t_builder	*builder;

	builder = malloc(sizeof(t_builder));
	if (builder == NULL)
		return (1);
	builder->head = NULL;
	builder->tail = NULL;
	builder->next = NULL;
	builder->flags = BUILDF_NONE;
	if (build->is_assign)
		builder->flags |= BUILDF_ASSIGN;
	if (build->head == NULL)
		build->head = builder;
	else
		build->tail->next = builder;
	build->tail = builder;
	return (0);
}

t_atom	*new_atom(const char *value, size_t len, t_atomtype type)
{
	t_atom	*atom;

	atom = malloc(sizeof(t_atom));
	if (atom == NULL)
		return (NULL);
	atom->type = type;
	atom->value = (char *)value;
	atom->len = len;
	atom->next = NULL;
	return (atom);
}

void	check_assignment(t_expand *build, t_atom *atom)
{
	size_t	i;

	if (build->tail->flags & BUILDF_EQ)
		return ;
	i = 0;
	while (i < atom->len)
	{
		if (build->tail->flags & BUILDF_LEFT)
		{
			if (atom->value[i] == '=')
				build->tail->flags |= BUILDF_EQ;
		}
		else if (ft_isalpha(atom->value[i]) || atom->value[i] == '_')
			build->tail->flags |= BUILDF_LEFT;
		i++;
	}
}

static int	push_new_atom(t_expand *build, const char *value, size_t len, t_atomtype type)
{
	t_atom	*new;

	new = new_atom(value, len, type);
	if (new == NULL)
		return (1);
	if (build->tail->head == NULL)
		build->tail->head = new;
	else
		build->tail->tail->next = new;
	build->tail->tail = new;
	if (type == ATOM_WILD && !(build->tail->flags & BUILDF_EQ))
		build->tail->flags |= BUILDF_WILD;
	if (build->tail->flags & BUILDF_ASSIGN && type == ATOM_LIT)
		check_assignment(build, build->tail->tail);
	return (0);
}

int	append_atom(t_expand *build, const char *value, size_t len, t_atomtype type)
{
	t_atom	*next;

	if (build->tail == NULL || build->tail->flags & BUILDF_FINISH)
	{
		if (new_builder(build) == 1)
			return (1);
	}
	if (type == ATOM_WILD && build->tail->tail
		&& build->tail->tail->type == ATOM_WILD)
	{
		build->tail->tail->len += len;
		return (0);
	}
	if (type == ATOM_LIT && len == 0 && build->tail->head)
		return (0);
	if (build->tail->head && build->tail->head->type == ATOM_LIT
		&& build->tail->head->len == 0)
	{
		next = build->tail->head->next;
		if (build->tail->tail == build->tail->head)
			build->tail->tail = next;
		free(build->tail->head);
		build->tail->head = next;
	}
	return (push_new_atom(build, value, len, type));
}

// TODO: this is a really complex code for norm, but must be moved to a function in a verbose way
int	multi_param(t_shell *shell, t_expand *build, char *val)
{
	size_t	i;
	size_t	strt;

	(void)shell;
	i = 0;
	strt = 0;
	while (val[i])
	{
		if (ft_isspace(val[i]) || val[i] == '*')
		{
			if (i > strt && append_atom(build, &val[strt], i - strt, ATOM_LIT) == 1)
				return (1);
			if (build->tail->head != NULL && ft_isspace(val[i]))
				build->tail->flags |= BUILDF_FINISH;
			while (ft_isspace(val[i]))
				i++;
			strt = i;
			while (val[i] == '*')
				i++;
			if (i > strt && append_atom(build, NULL, i - strt, ATOM_WILD) == 1)
				return (1);
			strt = i;
		}
		else
			i++;
	}
	if (i > strt && val[strt] && !ft_isspace(val[strt]))
		return (append_atom(build, &val[strt], i - strt, ATOM_LIT));
	return (0);
}

int	solve_param(t_shell *shell, t_seg *param, t_expand *build)
{
	char	*key;
	char	*value;

	if (param->slice.len == 1 && param->slice.begin[0] == '?')
	{
		if (build->last_status == NULL)
			build->last_status = ft_itoa(shell->last_status);
		if (build->last_status == NULL)
			return (1);
		return (append_atom(build, build->last_status,
				ft_strlen(build->last_status), ATOM_LIT));
	}
	key = malloc(sizeof(char) * (param->slice.len + 1));
	if (key == NULL)
		return (perror_malloc(), 1);
	ft_strlcpy(key, param->slice.begin, param->slice.len + 1);
	value = get_env_value(&shell->env_list, key);
	free(key);
	if (value == NULL || value[0] == '\0')
		return (0);
	if (param->flags & SEGF_QUOTED || build->tail->flags & BUILDF_EQ)
		return (append_atom(build, value, ft_strlen(value), ATOM_LIT));
	return (multi_param(shell, build, value));
}

int	get_builders(t_shell *shell, t_tok *tok, t_expand *build)
{
	t_seg		*cur;

	if (new_builder(build) == 1)
		return (1);
	cur = tok->seg_head;
	while (cur != NULL)
	{
		if (cur->type == SEG_TEXT && append_atom(build, cur->slice.begin,
				cur->slice.len, ATOM_LIT) == 1)
			return (1);
		else if (cur->type == SEG_WILDCARD)
		{
			if (append_atom(build, NULL, 1, ATOM_WILD) == 1)
				return (1);
		}
		else if (cur->type == SEG_PARAM && solve_param(shell, cur, build) == 1)
			return (1);
		cur = cur->next;
	}
	return (0);
}

int	build_literals(t_shell *shell, t_builder *builder, t_argv *argv)
{
	size_t	total_len;
	size_t	i;
	t_atom	*acur;
	char	*joined;

	(void)shell; // TODO: for set error
	total_len = 0;
	acur = builder->head;
	while (acur != NULL)
	{
		total_len += acur->len;
		acur = acur->next;
	}
	joined = malloc(sizeof(char) * (total_len + 1));
	if (joined == NULL)
		return (perror_malloc(), 1);
	acur = builder->head;
	i = 0;
	while (acur != NULL)
	{
		if (acur->type == ATOM_LIT)
			ft_strlcpy(joined + i, acur->value, acur->len + 1);
		else if (acur->type == ATOM_WILD)
			ft_memset(joined + i, '*', acur->len);
		i += acur->len;
		acur = acur->next;
	}
	joined[total_len] = '\0';
	if (new_argv_push(argv, joined) == 1)
		return (free(joined), 1);
	return (0);
}

int	build_expansion(t_shell *shell, t_expand *build, t_argv *argv)
{
	t_builder	*next;

	while (build->head != NULL)
	{
		if (build->head->flags & BUILDF_WILD
			&& !(build->tail->flags & BUILDF_EQ))
		{
			if (expand_wildcards(shell, build->head, argv) == 1)
				return (1);
		}
		else
		{
			if (build_literals(shell, build->head, argv) == 1)
				return (1);
		}
		next = build->head->next;
		// free_builder(&build->head);
		build->head = next;
	}
	return (0);
}

int	expansion(t_shell *shell, t_tok *tok, t_argv *argv, int is_assign)
{
	t_expand	build;

	build.head = NULL;
	build.tail = NULL;
	build.last_status = NULL;
	build.is_assign = is_assign;
	if (get_builders(shell, tok, &build) == 1)
		return (1); // TODO: free
	if (build.head->head == NULL)
		return (0); // TODO: free build list (only one empty node)
	(void)argv;

	t_builder *bcur = build.head;
	while (bcur != NULL)
	{
		printf("Builder: %p\n", bcur);
		t_atom *atom = bcur->head;
		while (atom != NULL)
		{
			if (atom->type == ATOM_WILD)
				printf("  Wildcard Atom [*]\n");
			else
				printf("  Literal Atom [%.*s]\n", (int)atom->len, atom->value);
			atom = atom->next;
		}
		bcur = bcur->next;
	}

	if (build_expansion(shell, &build, argv) == 1)
		return (1); // TODO: free
	if (build.last_status != NULL)
		free(build.last_status);
	return (0);
}
