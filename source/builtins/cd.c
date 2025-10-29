/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:40:58 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/29 17:51:38 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ============================================================================
 * PURPOSE: Change directory in parent shell; maintain PWD/OLDPWD env.
 * CONTROL FLOW:
 *   1) get_current_pwd -> snapshot previous dir (getcwd or PWD fallback)
 *   2) resolve_cd_target -> HOME | "-"(OLDPWD) | path; argc validation
 *   3) chdir(target) -> set error on failure
 *   4) update_oldpwd_env(old_pwd) -> OLDPWD=old_pwd
 *   5) ensure_pwd_env() -> PWD=<getcwd>
 *   6) set_last_status(0/1)
 * RELATIONSHIPS: Uses env_get_value/env_upsert; status via set_last_status.
 * EXTERNAL USAGE: Invoked by run_builtin (builtin_exec.c) for "cd".
 * NOTES: "cd -" prints new dir to stdout; errors don't exit the shell.
 * ========================================================================== */
#include "minishell.h"
/* PURPOSE: previous PWD; prefer getcwd, fallback to $PWD.
 * RETURN: malloc'd CWD or NULL; prints ENOMEM via perror_malloc. */
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

/* PURPOSE: Set OLDPWD to old_dir (move/update entry). PARAMS: @shell,@old_dir
 * RETURN: 0 ok, 1 on malloc error.  NOTES: No-op if old_dir NULL. */
static int	update_oldpwd_env(t_shell *shell, const char *old_dir)
{
	char	*oldpwd_line;

	if (!old_dir)
		return (0);
	oldpwd_line = ft_strjoin("OLDPWD=", old_dir);
	if (!oldpwd_line)
		return (perror_malloc(shell));
	if (env_upsert(&shell->env_list, oldpwd_line, oldpwd_line + 6) == 1)
	{
		free(oldpwd_line);
		return (perror_malloc(shell));
	}
	free(oldpwd_line);
	return (0);
}

/* PURPOSE: Refresh PWD with getcwd.
 * RETURN: 0 ok, 1 on error; sets status on error path. */
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
	if (env_upsert(&shell->env_list, pwd_line, pwd_line + 3) == 1)
		return (free(pwd_line), perror_malloc(shell));
	free(pwd_line);
	return (0);
}

/* PURPOSE: Resolve destination directory.	PARAMS: @shell,@argv
 * RETURN: malloc'd target dir or NULL; handles HOME, "-", argc>2 errors.
 * NOTES: Prints path on "cd -". Sets last_status on errors. */
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

/* PURPOSE: Entry point for "cd".	PARAMS: @shell,@argv
 * RETURN: 0 success, 1 fatal; sets last_status on success/failure.
 * BEHAVIOR: Snapshot old PWD -> resolve target -> chdir -> update env. */
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
		return (0);
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
