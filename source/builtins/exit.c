/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:44 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/11 22:01:34 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

// internal free
static void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->line)
		free(shell->line);
	if (shell->ast)
		free_exp_ast(&shell->ast);
	free_env_list(&shell->env_list);
}

/*	Check string = valid numeric argument for exit
	Accepts optional + or - sign, then digits only	*/
static int	is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	if (str[0] == '-' || str[0] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}
//! 	string to long long (ft_strtoll) => check this functions
//	Converts string to long long

static long long	ft_strtoll(const char *str)
{
	long long	res;
	int			sign;
	int			i;

	res = 0;
	sign = 1;
	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		i++;
	}
	return (res * sign);
}

static void	perror_numeric(t_shell *shell, const char *argv)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd((char *)argv, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
	free_shell(shell);
	exit(2);
}

/*	print "exit" before executing exit
 	handle: no args, 1 numeric, non-numeric & multiple
	set status variable for multiple args (no exit)

       	status = (unsigned char)status;
		exit code valid range [0, 255] unsigned 8-bit
*/
int	ft_exit(t_shell *shell, char *argv[])
{
	long long	status;

	status = 0;
	write(1, "exit\n", 5);
	if (!argv[1])
		status = shell->last_status;
	else if (!is_numeric(argv[1]))
		perror_numeric(shell, argv[1]);
	else if (argv[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		shell->last_status = 1;
		return (1);
	}
	else
		status = ft_strtoll(argv[1]);
	status = (unsigned char)status;
	rl_clear_history();
	free_shell(shell);
	exit(status);
}
