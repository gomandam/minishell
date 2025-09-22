/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/22 20:23:13 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

/*  Validate env assignment key (KEY=VAL).
    First char: alpha/_; rest: alnum/_. Must have '='. */
static int	export_key_valid(const char *s)
{
	int	i;

	if (!s || !*s)
		return (0);
	if (!(ft_isalpha(s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i] && s[i] != '=')
	{
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (s[i] == '=');
}

/* Remove env var by key from list.
 * Handles head/tail, frees node, updates size, clears envp cache. */
static void	env_remove_key(t_env_list *env, const char *key)
{
	t_env	*cur;
	t_env	*prev;
	size_t	len;

	len = 0;
	while (key[len] && key[len] != '=')
		len++;
	prev = NULL;
	cur = env->head;
	while (cur)
	{
		if (!ft_strncmp(cur->full, key, len) && cur->full[len] == '=')
		{
			if (prev)
				prev->next = cur->next;
			else
				env->head = cur->next;
			if (env->tail == cur)
				env->tail = prev;
			free(cur->full);
			free(cur);
			env->size--;
			break ;
		}
		prev = cur;
		cur = cur->next;
	}
	if (env->envp)
	{
		free(env->envp);
		env->envp = NULL;
	}
}

/* Handle one export arg: validate, remove old, make node, push.
   Only accepts KEY=VAL (must have '='), never bare KEY.
   Uses create_env_node, env_list_push. Error: print, set last_status. */
static int	export_one(t_shell *shell, const char *arg)
{
	char	*eq;
	char	*val;
	t_env	*node;
	char	*full;

	if (!export_key_valid(arg))
		return (export_perror_identifier(shell, (char *)arg), 1);
	eq = ft_strchr(arg, '=');
	val = NULL;
	if (eq != NULL)
		val = eq + 1;
	env_remove_key(&shell->env_list, arg);
	full = ft_strdup(arg);
	if (!full)
		return (perror_malloc(shell));
	node = create_env_node(full, val);
	if (!node)
	{
		free(full);
		return (perror_malloc(shell));
	}
	env_push(&shell->env_list, node);
	return (0);
}

/* Export builtin entry: loop argv, call export_one.
   If no args, print all env vars using export_print_all.
   Returns 0 success, 1 if any error. */
int	ft_export(t_shell *shell, char **argv)
{
	int	any_err;
	int	i;

	if (!argv || !argv[1])
		return (export_print_all(shell), 0);
	i = 1;
	any_err = 0;
	while (argv[i])
	{
		if (export_one(shell, argv[i]))
			any_err = 1;
		i++;
	}
	return (any_err);
}
