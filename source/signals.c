/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 04:09:07 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/11 15:11:11 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "minishell.h"

volatile sig_atomic_t	g_signum = 0;

static void	handle_repl_sigint(int s)
{
	g_signum = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	(void)s;
}

void	signals_repl(t_shell *shell)
{
	struct sigaction	sa;

	if (!shell->interactive)
		return ;
	ft_bzero(&sa, sizeof(sa));
	if (g_signum == SIGINT)
	{
		g_signum = 0;
		write(STDOUT_FILENO, "\n", 1);
	}
	sa.sa_handler = handle_repl_sigint;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

static void	handle_wait_sigint(int s)
{
	g_signum = SIGINT;
	(void)s;
}

void	signals_wait(t_shell *shell)
{
	struct sigaction	sa;

	if (!shell->interactive)
		return ;
	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = handle_wait_sigint;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
