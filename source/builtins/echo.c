/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:14 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/18 01:09:08 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

static int	echo_flag(char *argv)
{
	int	i;

	if (!argv || argv[0] != '-' || argv[1] != 'n')
		return (0);
	i = 2;
	while (argv[i] != '\0')
	{
		if (argv[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

static void	echo_print(char *argv[], int start)
{
	int	i;
	int	length;

	i = start;
	while (argv[i] != NULL)
	{
		length = 0;
		while (argv[i][length])
			length++;
		write(STDOUT_FILENO, argv[i], length);
		if (argv[i + 1] != NULL)
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
}

int	ft_echo(t_cmd *cmd)
{
	int		i;
	int		n_flag;
	char	**argv;

	i = 1;
	n_flag = 0;
	if (!cmd || !cmd->u_data.argv)
		return (0);
	argv = cmd->u_data.argv;
	while (argv[i] && echo_flag(argv[i]))
	{
		n_flag = 1;
		i++;
	}
	if (argv[i])
		echo_print(argv, i);
	if (!n_flag)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}
