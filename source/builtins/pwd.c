/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:03:55 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/27 18:43:31 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"
/*
** print working directory | current working directory
** Prints the current working directory to stdout.
** Returns 0 on success, 1 on failure.
	No need for t_shell args since !modify/read session state
*/
int	ft_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd == NULL)
	{
		perror("minishell: pwd: error retrieving current directory");
		return (1);
	}
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}
