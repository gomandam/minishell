/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 12:20:29 by migugar2          #+#    #+#             */
/*   Updated: 2025/10/01 13:56:24 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	perror_open(t_shell *shell, const char *file)
{
	char	*msg;

	msg = ft_strjoin("minishell: ", file);
	if (msg == NULL)
		return (perror_malloc(shell));
	perror(msg);
	free(msg);
	set_last_status(shell, 1);
	return (1);
}
