/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/05 02:59:47 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <errno-base.h>

int	get_env_paths(char ***path, char **envp)
{
	char	*tmp;
	size_t	i;

	i = 0;
	while (envp != NULL && envp[i] != NULL)
	{
		if (ft_strcmp(envp[i], "PATH=", 5) == 0)
		{
			*path = ft_split(envp[i] + 5, ':');
			if (*path == NULL)
				return (seterrno(ENOMEM), -1);
			i = 0;
			while (*path != NULL && (*path)[i] != NULL)
			{
				tmp = ft_strjoin((*path)[i], "/");
				if (tmp == NULL)
					return (ft_freestarr(path), seterrno(12), -1);
				ft_freestr(&(*path)[i]);
				(*path)[i++] = tmp;
			}
			return (0);
		}
		i++;
	}
	return (-1);
}

int	get_cmd_path(char **cmd_path, const char *cmd_name, char **envp)
{
	size_t	i;
	char	**paths;

	if (ft_strchr(cmd_name, '/') != NULL)
	{
		*cmd_path = ft_strdup(cmd_name);
		if (*cmd_path == NULL)
			return (seterrno(ENOMEM), -1);
		return (0);
	}
	paths = NULL;
	if (get_env_paths(&paths, envp) == -1)
		return (seterrno(ENOENT), -1);
	i = 0;
	while (paths != NULL && paths[i] != NULL)
	{
		*cmd_path = ft_strjoin(paths[i], cmd_name);
		if (*cmd_path == NULL)
			return (ft_freestarr(&paths), seterrno(ENOMEM), -1);
		if (access(*cmd_path, F_OK) == 0)
			return (ft_freestarr(&paths), seterrno(0), 0);
		ft_freestr(cmd_path);
		i++;
	}
	return (ft_freestarr(&paths), seterrno(ENOENT), -1);
}
