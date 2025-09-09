/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_exec1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/09 22:06:58 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_freestr(char **str)
{
	if (str != NULL && str != NULL)
	{
		free(*str);
		*str = NULL;
	}
	return (NULL);
}

void	ft_freestrarr(char ***arr)
{
	size_t	i;

	if (!arr || !*arr)
		return ;
	i = 0;
	while ((*arr)[i])
		free((*arr)[i++]);
	free(*arr);
	*arr = NULL;
}
