/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 03:04:34 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/29 17:52:58 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * PURPOSE: Implement POSIX echo with -n flag (multiple -n tokens supported).
 * CONTROL FLOW:
 *   1) Parse flags: accept one or more "-n" tokens (echo_flag)
 *   2) Print arguments separated by spaces (echo_print)
 *   3) If no -n, append newline
 *   4) Set last_status to 0 on success, 1 on write error
 * RELATIONSHIPS: Used by run_builtin (builtin_exec.c). Writes to provided FD.
 * EXTERNAL USAGE: builtin "echo".
 * NOTES: No interpretation of backslash escapes per 42 Minishell spec.
 * ========================================================================== */
#include "minishell.h"
/* PURPOSE: Check if argv is a valid -n flag (e.g., -n, -nnn).
 * PARAMS: @argv	RETURN: 1 if valid -n flag, else 0. */
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

/* PURPOSE: Write argv[start..] separated by a single space.
 * PARAMS: @argv,@start index,@out_fd	RETURN: 0 ok, 1 on write error. */
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

/* PURPOSE: Set shell->last_status based on error flag. */
static void	echo_last_status(t_shell *shell, int error)
{
	if (error)
		set_last_status(shell, 1);
	else
		set_last_status(shell, 0);
}

/* PURPOSE: Entry point for "echo".	PARAMS: @shell,@cmd (argv),@out_fd
 * RETURN: 0 ok, 1 on write error; sets last_status. */
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
