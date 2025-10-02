/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subsh_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 01:52:08 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/02 22:43:27 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_subsh(t_shell *shell, t_ast **subsh, int pipe_seq)
{
	signals_exec(shell);
	if (external_redirs(shell, &(*subsh)->u_data.subsh.redir) == 0)
		execute_ast(shell, &(*subsh)->u_data.subsh.child);
	free_exp_ast(subsh);
	if (shell->line)
		rl_clear_history();
	ft_freestr(&shell->line);
	free_env_list(&shell->env_list);
	if (!pipe_seq)
	{
		free_parse_ast(&shell->ast);
		exit(shell->last_status);
	}
}

int	execute_ast_subsh(t_shell *shell, t_ast **subsh)
{
	pid_t	pid;

	if (expand_subsh(shell, &(*subsh)->u_data.subsh) == 1)
		return (free_parse_ast(subsh), 1);
	pid = fork();
	if (pid == -1)
		return (perror_syscall(shell, "minishell: fork"));
	else if (pid == 0)
		run_subsh(shell, subsh, 0);
	wait_last_pid(shell, pid);
	free_exp_ast(subsh);
	return (0);
}
