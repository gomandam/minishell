/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subsh_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 01:52:08 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/02 05:07:07 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
static int	free_others_ast(t_ast **ast, t_ast *keep)
{
	if (ast == NULL || *ast == NULL)
		return (0);
	if (*ast == keep || *ast == keep->u_data.subsh.child)
		return (1);
	if ((*ast)->type == AST_PIPE || (*ast)->type == AST_AND_IF
		|| (*ast)->type == AST_OR_IF)
	{
		free_others_ast(&(*ast)->u_data.op.left, keep);
		free_others_ast(&(*ast)->u_data.op.right, keep);
		ft_close(&(*ast)->u_data.op.pipe_fd[0]);
		ft_close(&(*ast)->u_data.op.pipe_fd[1]);
	}
	else if ((*ast)->type == AST_SUBSH)
	{
		free_others_ast(&(*ast)->u_data.subsh.child, keep);
		free_redirs(&(*ast)->u_data.subsh.redir);
	}
	else if ((*ast)->type == AST_CMD)
		return (free_parse_ast_cmd(ast), 0);
	free(*ast);
	*ast = NULL;
	return (0);
}
*/

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
