/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:14 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 01:20:49 by migugar2         ###   ########.fr       */
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

static int	echo_print(char *argv[], int start, int out_fd)
{
	int	i;
	int	length;

	i = start;
	while (argv[i] != NULL)
	{
		length = 0;
		while (argv[i][length])
			length++;
		if (write(out_fd, argv[i], length) == -1)
			return (1);
		if (argv[i + 1] != NULL && write(out_fd, " ", 1) == -1)
			return (1);
		i++;
	}
	return (0);
}

static void	echo_last_status(t_shell *shell, int error)
{
	if (error)
		set_last_status(shell, 1);
	else
		set_last_status(shell, 0);
}

int	ft_echo(t_shell *shell, t_cmd *cmd, int out_fd)
{
	char	**argv;
	int		i;
	int		n_flag;
	int		error;

	i = 1;
	n_flag = 0;
	error = 0;
	if (!cmd || !cmd->u_data.argv)
		return (set_last_status(shell, 0), 0);
	argv = cmd->u_data.argv;
	while (argv[i] && echo_flag(argv[i]))
	{
		n_flag = 1;
		i++;
	}
	if (argv[i])
		error = echo_print(argv, i, out_fd);
	if (!n_flag && !error)
		if (write(out_fd, "\n", 1) == -1)
			error = 1;
	echo_last_status(shell, error);
	return (error);
}
// Same as Nested IF:
// 	if (!n_flag && !error && write(STDOUT_FILENO, "\n", 1) == -1)
// Test: echo -n foo > /dev/full   **output:  write error, space on device**
/* test: tokens, output redir (STDOUT)
echo foo bar			foo bar		0
echo -n hello			hello		0
echo				(newline)	0
echo -nnnn test			test		0
echo -z test			-z test		0
echo "fail" > /dev/full		(error)		1
echo "fail" > unwritable	(error)		1
*/
