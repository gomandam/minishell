/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 12:08:36 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/23 19:04:07 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	expand_wildcards(t_shell *shell, t_exp *exp, char *expanded, t_argv *argv)
{
	// TODO: handle wildcards, can push more than 1 value, when not find coincidences return the parsed tok
	(void)shell;
	(void)exp;
	if (new_argv_push(argv, expanded) == 1)
		return (free(expanded), 1);
	return (0);
}
