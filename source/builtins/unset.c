/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 22:31:04 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_identifier(const char *s)
{
	int	i;

	if (!s || !(ft_isalpha(s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static int	unset_perror_id(char *arg)
{
	size_t	len;

	len = ft_strlen(arg);
	write(STDERR_FILENO, "minishell: unset: `", 19);
	write(STDERR_FILENO, arg, len);
	write(STDERR_FILENO, "': not a valid identifier\n", 26);
	return (1);
}

int	ft_unset(t_shell *shell, char *argv[])
{
	int		i;
	int		error;

	if (!argv || !argv[1])
		return (set_last_status(shell, 0), 0);
	i = 1;
	error = 0;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
		{
			unset_perror_id(argv[i]);
			error = 1;
		}
		else
			env_remove(&shell->env_list, argv[i], ft_strlen(argv[i]));
		i++;
	}
	set_last_status(shell, error);
	return (error);
}
