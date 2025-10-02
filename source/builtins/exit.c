/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:44 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 21:58:22 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

static void	free_exit(t_shell *shell, t_ast **ast)
{
	if (!shell)
		return ;
	if (shell->line)
		ft_freestr(&shell->line);
	rl_clear_history();
	free_exp_ast(ast);
	if (shell->ast)
		free_parse_ast(&shell->ast);
	free_env_list(&shell->env_list);
}

static int	is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (!str || !str[i])
		return (0);
	while (str[i] && ft_isspace(str[i]))
		i++;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (!ft_isdigit(str[i]))
		return (0);
	while (str[i] && ft_isdigit(str[i]))
		i++;
	while (str[i] && ft_isspace(str[i]))
		i++;
	return (str[i] == '\0');
}

static int	exit_perror_numeric(t_shell *shell, t_ast **ast, char *argv)
{
	size_t	len;

	len = ft_strlen(argv);
	write(STDERR_FILENO, "minishell: exit: ", 17);
	write(STDERR_FILENO, argv, len);
	write(STDERR_FILENO, ": numeric argument required\n", 28);
	free_exit(shell, ast);
	exit(2);
	return (1);
}

static long long	exit_to_ll(t_shell *shell, t_ast **ast, char *str)
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
		if (sign == 1 && (res > (LLONG_MAX - (str[i] - '0')) / 10))
			exit_perror_numeric(shell, ast, str);
		else if (sign == -1 && (-res < (LLONG_MIN + (str[i] - '0')) / 10))
			exit_perror_numeric(shell, ast, str);
		res = res * 10 + (str[i] - '0');
		i++;
	}
	return (res * sign);
}

int	ft_exit(t_shell *shell, t_ast	**ast)
{
	uint8_t	status;

	status = 0;
	write(2, "exit\n", 5);
	if (!(*ast)->u_data.cmd.u_data.argv[1])
		status = shell->last_status;
	else if (!is_numeric((*ast)->u_data.cmd.u_data.argv[1]))
		exit_perror_numeric(shell, ast, (*ast)->u_data.cmd.u_data.argv[1]);
	else if ((*ast)->u_data.cmd.u_data.argv[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		set_last_status(shell, 1);
		return (0);
	}
	else
		status = (uint8_t)exit_to_ll(shell, ast,
				(*ast)->u_data.cmd.u_data.argv[1]);
	free_exit(shell, ast);
	exit(status);
}
