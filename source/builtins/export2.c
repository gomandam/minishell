/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/18 01:04:00 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	set environment variables -> available to child process
	e.g. if 'unset' a function, 'export' to access
 */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

static void	print_export_var(const char *var)
{
	const char	*eq;
	int			i;

	if (!var)
		return ;
	ft_putstr_fd("declare -x ", 1);
	eq = ft_strchr(var, '=');
	if (eq)
	{
		i = 0;
		while (&var[i] != eq)
			ft_putchar_fd(var[i++], 1);
		ft_putstr_fd("=\"", 1);
		ft_putstr_fd(eq + 1, 1);
		ft_putstr_fd("\"\n", 1);
	}
	else
	{
		ft_putstr_fd(var, 1);
		ft_putchar_fd('\n', 1);
	}
}

void	print_exports_char(char **envp, char c)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (envp[i] && envp[i][0] == c)
			print_export_var(envp[i]);
		i++;
	}
}

void	print_exports_nonalpha(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (!ft_isalpha(envp[i][0]))
			print_export_var(envp[i]);
		i++;
	}
}

// adds a string to a NULL-terminated char **array.
// returns new array or NULL on error.
char	**add_str_to_array(char **array, char *str)
{
	int		len;
	char	**new_array;
	int		i;

	len = 0;
	while (array && array[len])
		len++;
	new_array = malloc(sizeof(char *) * (len + 2));
	if (!new_array)
		return (NULL);
	i = 0;
	while (i < len)
	{
		new_array[i] = ft_strdup(array[i]);
		i++;
	}
	new_array[i] = ft_strdup(str);
	new_array[i + 1] = NULL;
	if (array)
		free_array(array);
	return (new_array);
}

// returns 1 if str is in array, 0 otherwise. Compares up to '=' if present.
int	str_in_array(char **array, char *str)
{
	int		i;
	int		cmp_len;

	if (!array || !str)
		return (0);
	cmp_len = 0;
	while (str[cmp_len] && str[cmp_len] != '=')
		cmp_len++;
	i = 0;
	while (array[i])
	{
		if (!ft_strncmp(array[i], str, cmp_len) && (array[i][cmp_len] == '='
			|| array[i][cmp_len] == '\0'))
			return (1);
		i++;
	}
	return (0);
}
