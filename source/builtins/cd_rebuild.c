/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_rebuild.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:02:59 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/25 03:06:21 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

void	cd_error(char **argv[2])
{
	if ()
	//No argument (cd): goes to $HOME or errors if not set.
	//Single argument (cd dirname): checks existence, directory type, and calls chdir.
	//Error messages for missing HOME, not a directory, file not found.
}

// separate command logic from error handling, updates environment variables

int	ft_cd(t_shell *shell, char *argv[])
{
	char	*cwd;

	cwd = getcwd();	
	if (!argv[1])
		return(printf("minishell: cd: error\n"), 0);
	if (chdir(argv[1]) != 0) 	// chdir function returns 0 on success and -1 on failure. Anything other than 0 is an error occurence.
		return (0);
	if (shell->prompt) 
		free(shell->prompt);
	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (0);
	
}

	// 1. Get command arguments
	// 2. Retrieve HOME from environment
	// 3. Check for arguments if not 'HOME'. If home doesnt exist then error
	// 4. ft_strncmp to find '-' and implement going back to previous directory
	// 5. Call error handler for cd logic and chdir
	// 5. Use get_env_value() to set OLDPWD
	// 6. If chdir successful, update OLDPWD, if doesn't exist then print error (also check for exit status)
	// 7. Update PWD to new current directory, and print
	// 8. Free and return
	//

Getcwd to find path when entering CD.

Know which directory are we going to

Check for arguments if not 'HOME'
Home doesnt exist then error

Strncmp to find '-'

Use get_env_value() to set OLDPWD
if doesnt exist then print error

Print
//

int	cd_command(char ** cmd_info, t_data *data)
{
	char	*target_dir;
	char	*current_pwd;

	current_pwd = getcwd(NULL,0);
	if (!current_pwd)
		current_pwd = get_env_value(data->env, "PWD");
	target_dir = get_cd_target(data, cmd_info);
	if (!target_dir)
	{
		cleanup_cd_pwd(data, current_pwd);
		return (1);
	}
	if (chdir(target_dir) == -1)
	{
		perror("cd");
		cleanup_cd_pwd(data, current_pwd);
		return (1);
	}
	if (current_pwd)
		update_oldpwd(data, current_pwd);
	else
		update_oldpwd(data, "");
	update_pwd(data);
	cleanup_cd_pwd(data, current_pwd);
	return (0);
}
