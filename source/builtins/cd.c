/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:02:59 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/27 20:34:43 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

/* Get current working directory with PWD env_get_value()
   Returns malloc'd string. NULL when fail. */
static char	*get_current_pwd(t_shell *shell)
{
	char	*cwd;
	char	*pwd_value;

	cwd = getcwd(NULL, 0);
	if (cwd)
		return (cwd);
	if (errno == ENOMEM)
	{
		perror_malloc(shell);
		return (NULL);
	}
	pwd_value = env_get_value(&shell->env_list, "PWD");
	if (pwd_value)
		return (ft_strdup(pwd_value));
	return (NULL);
}

/* OLDPWD environment variable with given directory.
   Uses env_upsert() pattern from init.c ensure_pwd_env() */
static int	update_oldpwd_env(t_shell *shell, const char *old_dir)
{
	char	*oldpwd_line;

	if (!old_dir)
		return (0);
	oldpwd_line = ft_strjoin("OLDPWD=", old_dir);
	if (!oldpwd_line)
		return (perror_malloc(shell));
	if (env_upsert(&shell->env_list, oldpwd_line, oldpwd_line + 7) == 1)
	{
		free(oldpwd_line);
		return (perror_malloc(shell));
	}
	free(oldpwd_line);
	return (0);
}

/* Updates PWD environment variable with current directory.
   ensure_pwd_env() from init.c exactly. */
static int	ensure_pwd_env(t_shell *shell)
{
	char	*cwd;
	char	*pwd_line;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		if (errno == ENOMEM)
			return (perror_malloc(shell));
		perror("minishell: cd: getcwd");
		return (1);
	}
	pwd_line = ft_strjoin("PWD=", cwd);
	free(cwd);
	if (!pwd_line)
		return (perror_malloc(shell));
	if (env_upsert(&shell->env_list, pwd_line, pwd_line + 4) == 1)
		return (free(pwd_line), perror_malloc(shell));
	free(pwd_line);
	return (0);
}

/*  Resolve cd target: no args->HOME, "-"->OLDPWD, else->path.
    returns malloc target or NULL on error. Prints to stdout for "cd -". */
static char	*resolve_cd_target(t_shell *shell, char **argv)
{
	char	*target;

	if (!argv[1])
	{
		target = env_get_value(&shell->env_list, "HOME");
		if (!target)
			return (ft_putstr_fd("minishell: cd: HOME not set\n", 2),
				set_last_status(shell, 1), NULL);
		return (ft_strdup(target));
	}
	if (argv[2])
		return (ft_putstr_fd("minishell: cd: too many arguments\n", 2),
			set_last_status(shell, 1), NULL);
	if (!ft_strcmp(argv[1], "-"))
	{
		target = env_get_value(&shell->env_list, "OLDPWD");
		if (!target)
		{
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
			return (set_last_status(shell, 1), NULL);
		}
		ft_putendl_fd(target, 1);
		return (ft_strdup(target));
	}
	return (ft_strdup(argv[1]));
}

/* Main ft_cd: change directory and update PWD/OLDPWD.
    env functions: env_get_value, env_upsert */
int	ft_cd(t_shell *shell, char **argv)
{
	char	*old_pwd;
	char	*target_dir;

	old_pwd = get_current_pwd(shell);
	target_dir = resolve_cd_target(shell, argv);
	if (!target_dir)
		return (free(old_pwd), 1);
	if (chdir(target_dir) == -1)
	{
		perror("minishell: cd");
		free(old_pwd);
		free(target_dir);
		set_last_status(shell, 1);
		return (1);
	}
	free(target_dir);
	if (old_pwd)
	{
		update_oldpwd_env(shell, old_pwd);
		free(old_pwd);
	}
	if (ensure_pwd_env(shell) != 0)
		return (set_last_status(shell, 1), 1);
	return (set_last_status(shell, 0), 0);
}
