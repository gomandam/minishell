/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 19:12:32 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/18 12:16:47 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>

void	signals_heredoc(t_shell *shell)
{
	struct sigaction	sa;

	if (!shell->interactive)
		return ;
	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = set_g_signum;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

ssize_t	heredoc_prompt(t_shell *shell, char **line, const char *delim)
{
	ssize_t	bytes_read;

	write(STDOUT_FILENO, HEREDOC_PROMPT, HEREDOC_PROMPT_LEN);
	bytes_read = get_next_line(STDIN_FILENO, line);
	if (bytes_read == 0)
		pwarn_heredoceof(delim);
	else if (bytes_read == -1)
	{
		if (g_signum == SIGINT)
			set_last_status(NULL, 130);
		else
			perror_syscall(shell, NULL);
	}
	return (bytes_read);
}

int	heredoc_fill(t_shell *shell, const char *delim, int pipefd[2], int quoted)
{
	char	*line;
	ssize_t	bytes_read;

	line = NULL;
	while (1)
	{
		bytes_read = heredoc_prompt(shell, &line, delim);
		if (bytes_read == 0)
			break ;
		else if (bytes_read == -1)
			return (ft_freestr(&line), 1);
		if (ft_strncmp(line, delim, bytes_read) == '\n')
			break ;
		if (!quoted)
		{
			if (heredoc_write(shell, line, pipefd[1]) == 1)
				return (ft_freestr(&line), 1);
		}
		else
			write(pipefd[1], line, bytes_read);
		ft_freestr(&line);
	}
	return (ft_freestr(&line), 0);
}

static int	check_quoted(t_tok *word)
{
	t_seg	*cur;

	cur = word->seg_head;
	while (cur != NULL)
	{
		if (cur->flags & SEGF_QUOTED)
			return (1);
		cur = cur->next;
	}
	return (0);
}

int	heredoc_redir(t_shell *shell, t_redir *redir)
{
	char	*delim;
	int		quoted;
	int		pipefd[2];
	int		status;

	quoted = check_quoted(redir->u_data.word);
	delim = simple_expansion(redir->u_data.word);
	free_tok(&redir->u_data.word);
	if (delim == NULL)
		return (perror_malloc(shell), 1);
	if (pipe(pipefd) == -1)
		return (perror_syscall(shell, "minishell: pipe"));
	signals_heredoc(shell);
	status = heredoc_fill(shell, delim, pipefd, quoted);
	signals_repl(shell);
	ft_close(&pipefd[1]);
	free(delim);
	if (status == 1)
		return (ft_close(&pipefd[0]), 1);
	redir->fd = pipefd[0];
	return (0);
}
