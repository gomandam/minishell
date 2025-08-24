/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 12:08:36 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/24 22:24:03 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	wildcard_recursive(const char *pattern, const char *str)
{
	if (*pattern == '\0' && *str == '\0')
		return (1);
	if (*pattern == '*' && *(pattern + 1) != '\0' && *str == '\0')
		return (0);
	if (*pattern == '*')
		return (wildcard_recursive(pattern + 1, str)
			|| wildcard_recursive(pattern, str + 1));
	if (*pattern == *str)
		return (wildcard_recursive(pattern + 1, str + 1));
	return (0);
}

int	wildcard_match(const char *pattern, const char *str)
{
	if (str[0] == '.' && (pattern[0] != '.' || str[1] == '.' || str[1] == '\0'))
		return (0);
	return (wildcard_recursive(pattern, str));
}

static int	wildcard_solve(t_shell *shell, char *expanded, t_argv *argv)
{
	DIR				*cur;
	struct dirent	*entry;

	(void)shell; // TODO: for set error
	if (ft_strchr(expanded, '/') != NULL)
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
		if (wildcard_match(expanded, entry->d_name))
		{
			if (new_argvdup_push(argv, entry->d_name) == 1)
				return (perror_malloc(), closedir(cur), 1);
		}
		entry = readdir(cur);
	}
	return (closedir(cur), 0);
}

int	expand_wildcards(t_shell *shell, char *expanded, t_argv *argv)
{
	size_t	initial_argc;
	char	*check_str;

	initial_argc = argv->argc;
	check_str = expanded;
	if (expanded[0] == '.' && expanded[1] == '/')
		check_str = check_str + 2;
	if (wildcard_solve(shell, check_str, argv) == 1)
		return (free(expanded), 1);
	if (initial_argc == argv->argc)
	{
		if (new_argv_push(argv, expanded) == 1)
			return (free(expanded), 1);
	}
	else
		free(expanded);
	return (0);
}
