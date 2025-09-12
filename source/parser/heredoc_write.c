/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_write.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 00:06:23 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/12 17:58:10 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// TODO: move
char	*get_env_n_value(t_env_list *env_list, const char *key, size_t n)
{
	t_env	*cur;

	cur = env_list->head;
	while (cur != NULL)
	{
		if (ft_strncmp(cur->full, key, n + 1) == ('=' - key[n]))
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}

static void	heredoc_writelit(int fd, char *line, size_t len)
{
	if (len > 0)
		write(fd, line - len, len);
}

static int	heredoc_exp(t_shell *shell, char **key, char **val, char **status)
{
	size_t	i;

	(*key)++;
	if ((*key)[0] == '?')
	{
		(*key)++;
		if (*status == NULL)
			*status = ft_itoa(shell->last_status);
		if (*status == NULL)
			return (perror_malloc(shell));
		*val = *status;
		return (0);
	}
	i = 0;
	while (ft_isalnum((*key)[i]) || (*key)[i] == '_')
		i++;
	*val = get_env_n_value(&shell->env_list, *key, i);
	*key += i;
	return (0);
}

static void	heredoc_writeexp(int fd, char *val)
{
	if (val)
		write(fd, val, ft_strlen(val));
}

int	heredoc_write(t_shell *shell, char *line, int fd)
{
	char	*status_str;
	size_t	len;
	char	*val;

	status_str = NULL;
	len = 0;
	val = NULL;
	fd = STDOUT_FILENO;
	while (line && *line != '\0')
	{
		if (is_param_start(line))
		{
			heredoc_writelit(fd, line, len);
			if (heredoc_exp(shell, &line, &val, &status_str))
				return (ft_freestr(&status_str), 1);
			heredoc_writeexp(fd, val);
			len = 0;
		}
		else
		{
			line++;
			len++;
		}
	}
	return (heredoc_writelit(fd, line, len), ft_freestr(&status_str), 0);
}

