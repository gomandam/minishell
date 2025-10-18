/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 00:46:43 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/18 00:48:02 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * FILE: cmd_path.c
 * ============================================================================
 * PURPOSE: Command path resolver - searches PATH for executables and handles
 *          absolute/relative paths.
 *
 * CONTROL FLOW:
 *   resolve_cmd_path
 *     ├─> [if contains '/'] return as-is (absolute/relative)
 *     └─> [else] get_cmd_path
 *          └─> [for each PATH dir] join_path_cmd + access(F_OK)
 *
 * EXTERNAL USAGE: run_external [cmd_exec.c]
 * NOTE: Returns malloc'd path string; caller must free
 * ========================================================================== */

#include "minishell.h"

/* FUNCTION: join_path_cmd
 * PURPOSE: Concatenate directory path and command name with '/'.
 * PARAMS: @shell - state, @dst_path - output, @dir - directory, @cmd - command
 * RETURN: 0 on success, 1 on malloc failure
 * NOTE: Allocates new string "dir/cmd"; caller must free  */
static int	join_path_cmd(t_shell *shell, char **dst_path, const char *dir,
	const char *cmd)
{
	size_t	dir_len;
	size_t	cmd_len;

	dir_len = ft_strlen(dir);
	cmd_len = ft_strlen(cmd);
	*dst_path = malloc(sizeof(char) * (dir_len + 1 + cmd_len + 1));
	if (*dst_path == NULL)
		return (perror_malloc(shell));
	ft_strlcpy(*dst_path, dir, dir_len + 1);
	(*dst_path)[dir_len] = '/';
	ft_strlcpy(*dst_path + dir_len + 1, cmd, cmd_len + 1);
	return (0);
}

/* FUNCTION: get_cmd_path
 * PURPOSE: Search PATH directories for executable command.
 * PARAMS: @shell - state, @dst - output path, @cmd - command, @paths - PATH array
 * RETURN: 0 if found, 1 if not found
 * BEHAVIOR: Iterate paths, join with cmd, test access(F_OK), return first match
 * NOTE: Sets *dst to malloc'd path on success; error 127 if not found  */
int	get_cmd_path(t_shell *shell, char **dst, const char *cmd, char **paths)
{
	char	*tmp;
	size_t	i;

	i = 0;
	while (paths != NULL && paths[i] != NULL)
	{
		if (join_path_cmd(shell, &tmp, paths[i], cmd) == 1)
			return (1);
		if (access(tmp, F_OK) == 0)
		{
			*dst = tmp;
			return (0);
		}
		ft_freestr(&tmp);
		i++;
	}
	return (perror_cmdnotfound(shell, cmd));
}

/* FUNCTION: resolve_cmd_path
 * PURPOSE: Main resolver - handles absolute/relative paths or searches PATH.
 * PARAMS: @shell - state, @dst - output path, @cmd - command name
 * RETURN: 0 on success, 1 on error
 * BEHAVIOR: If contains '/', return as-is; else split PATH and search
 * NOTE: Returns malloc'd string in *dst; checks if PATH exists  */
int	resolve_cmd_path(t_shell *shell, char **dst, const char *cmd)
{
	char	**paths;

	if (ft_strchr(cmd, '/') != NULL)
	{
		*dst = ft_strdup(cmd);
		if (*dst == NULL)
			return (perror_malloc(shell));
		return (0);
	}
	if (shell->env_list.path_dir == NULL || *shell->env_list.path_dir == NULL)
		return (perror_cmdnotfound(shell, cmd));
	paths = ft_split(*shell->env_list.path_dir, ':');
	if (paths == NULL)
		return (perror_malloc(shell));
	if (get_cmd_path(shell, dst, cmd, paths) == 1)
		return (ft_freestrarr(&paths), 1);
	ft_freestrarr(&paths);
	return (0);
}
