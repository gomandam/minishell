/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/12 04:12:47 by gomandam         ###   ########.fr       */
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

static int	matches_char(const char *var, char c)
{
	if (!var || !*var)
		return (0);
	return (*var == c);
}

void	print_exports_char(char **envp, char c)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (matches_char(envp[i], c))
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

int	valid_export_name(const char *s)
{
	int	i;

	if (!s || !*s)
		return (0);
	if (!ft_isalpha(*s) && *s != '_')
		return (0);
	i = 1;
	while (s[i] && s[i] != '=')
	{
		if (!ft_isalnum(s[i]) && s[i] != '_')
			return (0);
		i++;
	}
	return (1);
}
