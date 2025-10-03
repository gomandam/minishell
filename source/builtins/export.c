/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/10/02 22:26:04 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	export_perror_identifier(t_shell *shell, char *argv)
{
	ft_putstr_fd("minishell: export: '", 2);
	ft_putstr_fd(argv, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	set_last_status(shell, 1);
}

static int	env_update_move(t_env_list *env_list, t_env *found, t_env *prev)
{
	if (env_list->envp != NULL)
	{
		free(env_list->envp);
		env_list->envp = NULL;
	}
	if (found == env_list->tail)
		return (0);
	if (prev)
		prev->next = found->next;
	else
		env_list->head = found->next;
	if (env_list->tail)
		env_list->tail->next = found;
	else
		env_list->head = found;
	env_list->tail = found;
	found->next = NULL;
	return (0);
}

int	env_upsert(t_env_list *env_list, const char *full, const char *val)
{
	t_env	**found;
	t_env	*new_node;
	char	*full_dup;
	t_env	*prev;

	found = env_find(env_list, full, val - full, &prev);
	if (found != NULL && *found != NULL)
	{
		full_dup = ft_strdup(full);
		if (full_dup == NULL)
			return (1);
		free((*found)->full);
		(*found)->full = full_dup;
		(*found)->value = full_dup + (val - full) + 1;
		return (env_update_move(env_list, *found, prev));
	}
	full_dup = ft_strdup(full);
	if (full_dup == NULL)
		return (1);
	new_node = create_env_node(full_dup, full_dup + (val - full) + 1);
	if (new_node == NULL)
		return (free(full_dup), 1);
	env_push(env_list, new_node);
	return (0);
}

/*  Validate env assignment key (KEY=VAL).
    First char: alpha/_; rest: alnum/_. Must have '='. */
static int	is_key_valid(const char *s)
{
	int	i;

	if (!(ft_isalpha(s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i] && s[i] != '=')
	{
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

/* Export builtin entry: loop argv, call export_one.
   If no args, print all env vars using export_print_all. */
int	ft_export(t_shell *shell, char **argv, int out_fd)
{
	int		i;
	char	*val;

	if (!argv || !argv[1])
		return (export_print_all(shell, out_fd));
	i = 1;
	set_last_status(shell, 0);
	while (argv[i])
	{
		if (!is_key_valid(argv[i]))
			export_perror_identifier(shell, argv[i]);
		else
		{
			val = ft_strchr(argv[i], '=');
			if (val != NULL)
			{
				if (env_upsert(&shell->env_list, argv[i], val) == 1)
					return (perror_malloc(shell), 1);
			}
		}
		i++;
	}
	return (0);
}
