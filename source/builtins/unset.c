/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 01:19:49 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*	Unset values & attributes of variables.
 *	Removes access from the current shell session.
 *	Does NOT affect the parent of the shell.	*/

#include "../../include/minishell.h"
#include "../../libft/libft.h"

// OLD refactor of unset -> re-implemented again
static int	is_valid_identifier(const char *s)
{
	int	i;

	if (!s || !(ft_isalpha(s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static int	unset_perror_id(char *arg)
{
	size_t	len;

	len = ft_strlen(arg);
	write(STDERR_FILENO, "minishell: unset: `", 19);
	write(STDERR_FILENO, arg, len);
	write(STDERR_FILENO, "': not a valid identifier\n", 26);
	return (1);
}

int	ft_unset(t_shell *shell, char *argv[])
{
	int		i;
	int		error;

	if (!argv || !argv[1])
		return (set_last_status(shell, 0), 0);
	i = 1;
	error = 0;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
		{
			unset_perror_id(argv[i]);
			error = 1;
		}
		else
			env_remove(&shell->env_list, argv[i], ft_strlen(argv[i]));
		i++;
	}
	set_last_status(shell, error);
	return (error);
}

/* TEST for last_status
export MYVAR=42
unset MYVAR

export A=1 B=2
unset A B

export GOOD=ok
unset GOOD 2BAD

unset

unset 1 2 3

unset ""

unset -f MYVAR

unset DOES_NOT_EXIST

unset 1INVALID

unset BAD-NAME
unset "BAD NAME"

unset PATH
*/
// Find env node by key
/*
static int	find_env_node(t_env_list *env_list, const char *key,
	t_env **found, t_env **prev)
{
	t_env	*cur;
	size_t	keylen;

	*prev = NULL;
	cur = env_list->head;
	keylen = ft_strlen(key);
	while (cur)
	{
		if (ft_strncmp(cur->full, key, keylen) == 0
			&& cur->full[keylen] == '=')
		{
			*found = cur;
			return (1);
		}
		*prev = cur;
		cur = cur->next;
	}
	*found = NULL;
	return (0);
}
*/
/*
	unset: implements the 'unset' built-in.
	argv[0] = "unset", argv[1..n] = var names to unset
	Returns 1 if any variable was removed, 0 otherwise.	*/
/*
int	ft_unset(t_shell *shell, char *argv[])
{
	int		i;
	size_t	keylen;

	if (!argv || !argv[1])
		return (0);
	i = 1;
	while (argv[i])
	{
		keylen = ft_strlen(argv[i]);
		if (keylen != 0)
			env_remove(&shell->env_list, argv[i], keylen);
		i++;
	}
	return (0);
}
*/
/*
int	unset(char *argv[], char *envp[])
{
	int		a;
	int		b;
	char	*var;

	a = 0;
	if (!argv[0])
		return (0);
	var = argv[1];
	while (envp[a])
	{
		if (ft_strncmp(envp[a], var, ft_strlen(var)) == 0
			&& envp[a][ft_strlen(var) == '='])
		{
			b = a;
			while (envp[b])
			{
				envp[b] = envp[b + 1];
				b++;
			}
			return (1);
		}
		a++;
	}
	printf("minishell: unset: variable %s not found\n", var);
	return (1);
}
*/
