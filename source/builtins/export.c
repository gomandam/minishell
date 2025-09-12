/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/12 04:10:41 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	set environment variables -> available to child process
	e.g. if 'unset' a function, 'export' to access	*/

#include "../../include/minishell.h"
#include "../../libft/libft.h"

// TO DO: ft_strndup();  str_in_array();  add_str_to_array();
// Print all export vars, grouped and sorted
static void	export_no_args(char **envp)
{
	char	c;

	c = 'A';
	while (c <= 'Z')
	{
		print_exports_char(envp, c);
		c++;
	}
	print_exports_nonalpha(envp);
}

static void	export_error(const char *argv)
{
	ft_putstr_fd("minishell: export: '", 2);
	ft_putstr_fd((char *)argv, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

// Handles export logic for a single argv
static int	handle_export_var(t_shell *shell, const char *argv)
{
	char	*eq;
	char	*name;

	eq = ft_strchr(argv, '=');
	if (eq)
	{
		name = ft_strndup(argv, eq - argv);
		if (!valid_export_name(name))
		{
			export_error(argv);
			free(name);
			return (1);
		}
		shell->env_list.head = rm_env_var(shell, name);
		shell->env_list.head = add_str_to_array(shell->env_list.head, argv);
		free(name);
	}
	else
	{
		if (!valid_export_name(argv))
			return (export_error(argv), 1);
		if (!str_in_array(shell->env_list.head, argv))
			shell->env_list.head = add_str_to_array(shell->env_list.head, argv);
	}
	return (0);
}

int	ft_export(t_shell *shell, char **argv)
{
	int	i;
	int	error;

	if (!argv || !argv[0])
		return (1);
	if (!argv[1])
		return (export_no_args(shell->env_list.envp), 0);
	i = 1;
	error = 0;
	while (argv[i])
	{
		if (handle_export_var(shell, argv[i]))
			error = 1;
		i++;
	}
	if (error)
		return (1);
	return (0);
}
