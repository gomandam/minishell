/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 19:55:15 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/09 20:01:51 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	perror_cmdnotfound(t_shell *shell, const char *cmd)
{
	write(STDERR_FILENO, "minishell: command not found: ", 30);
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, "\n", 1);
	if (shell)
		shell->last_status = 127;
	return (1);
}
