/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 12:08:36 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/03 13:51:25 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_atomtype	atom_peek(t_atom *atom, size_t offset, char *out)
{
	while (atom && offset >= atom->len)
	{
		offset -= atom->len;
		atom = atom->next;
	}
	if (atom == NULL)
		return (ATOM_END);
	else if (atom->type == ATOM_WILD)
	{
		if (out != NULL)
			*out = '*';
		return (ATOM_WILD);
	}
	else if (atom->type == ATOM_LIT)
	{
		if (out != NULL)
			*out = atom->value[offset];
		return (ATOM_LIT);
	}
	return (ATOM_END);
}

void	atom_advance(t_atom **atom, size_t *offset)
{
	t_atom	*cur;
	size_t	k;

	cur = *atom;
	k = *offset;
	while (cur && k >= cur->len)
	{
		k -= cur->len;
		cur = cur->next;
	}
	if (cur == NULL)
	{
		*atom = NULL;
		*offset = k;
		return ;
	}
	k++;
	if (k >= cur->len)
	{
		cur = cur->next;
		k = 0;
	}
	*offset = k;
	*atom = cur;
}

int	wildcard_recursive(const char *str, t_atom *atom, size_t offset)
{
	char	c;
	t_atom	*newatom;
	size_t	newoff;

	if (atom_peek(atom, offset, &c) == ATOM_END || c == '\0')
		return (*str == '\0');
	if (atom_peek(atom, offset, NULL) == ATOM_WILD)
	{
		newatom = atom;
		newoff = offset;
		while (atom_peek(newatom, newoff, NULL) == ATOM_WILD)
			atom_advance(&newatom, &newoff);
		if (wildcard_recursive(str, newatom, newoff))
			return (1);
		while (*str)
		{
			if (wildcard_recursive(str + 1, newatom, newoff))
				return (1);
			str++;
		}
		return (0);
	}
	if (*str == '\0' || *str != c)
		return (0);
	atom_advance(&atom, &offset);
	return (wildcard_recursive(str + 1, atom, offset));
}

int	wildcard_match(const char *str, t_atom **start, size_t *offset)
{
	char	c;

	if (str[0] == '.' && (str[1] == '\0' || (str[1] == '.' && str[2] == '\0')
			|| (atom_peek(*start, *offset, &c) && c != '.')))
		return (0);
	return (wildcard_recursive(str, *start, *offset));
}

int	start_wildcard(t_builder *builder, t_atom **start, size_t *offset)
{
	t_atom	*cur;
	size_t	k;
	char	c;

	cur = builder->head;
	k = 0;
	while (atom_peek(cur, k, &c) && c == '.'
		&& atom_peek(cur, k + 1, &c) && c == '/')
	{
		atom_advance(&cur, &k);
		atom_advance(&cur, &k);
		while (atom_peek(cur, k, &c) && c == '/')
			atom_advance(&cur, &k);
	}
	*start = cur;
	*offset = k;
	while (atom_peek(cur, k, &c))
	{
		if (c == '/')
			return (1);
		atom_advance(&cur, &k);
	}
	return (0);
}

int	wildcard_solve(t_shell *shell, t_builder *builder, t_argv *argv)
{
	DIR				*cur;
	struct dirent	*entry;
	t_atom			*start;
	size_t			offset;

	(void)shell;
	if (start_wildcard(builder, &start, &offset) == 1)
		return (0);
	cur = opendir(".");
	if (cur == NULL) // bash don't print error for permissions denied or something on wildcard match, only return and append the original input word
	{
		if (errno == ENOMEM)
			return (perror_malloc(), 1);
		return (errno = 0, 0);
	}
	entry = readdir(cur);
	while (entry != NULL)
	{
		if (wildcard_match(entry->d_name, &start, &offset))
		{
			if (new_argvdup_push(argv, entry->d_name) == 1)
				return (perror_malloc(), closedir(cur), 1);
		}
		entry = readdir(cur);
	}
	return (closedir(cur), 0);
}

int	expand_wildcards(t_shell *shell, t_builder *builder, t_argv *argv)
{
	size_t	initial_argc;

	initial_argc = argv->argc;
	if (wildcard_solve(shell, builder, argv) == 1)
		return (1);
	if (argv->argc == initial_argc)
	{
		if (build_literals(shell, builder, argv) == 1)
			return (1);
	}
	return (0);
}
