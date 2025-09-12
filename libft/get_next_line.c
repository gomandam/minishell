/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 16:18:35 by migugar2          #+#    #+#             */
/*   Updated: 2025/09/12 14:19:29 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

ssize_t	save_in_static(char **saved, char **empty, char *new_line, size_t line)
{
	char	*new_saved;

	new_line++;
	new_saved = NULL;
	if (new_line[0] != '\0')
	{
		new_saved = ft_strdup(new_line);
		if (new_saved == NULL)
			return (ft_freestr(empty), ft_freestr(saved), -1);
	}
	ft_freestr(saved);
	*saved = new_saved;
	return ((ssize_t)line);
}

static ssize_t	extract_line(char **saved, char **empty)
{
	char	*new_line_pos;
	size_t	line_len;

	new_line_pos = ft_strchr(*saved, '\n');
	if (new_line_pos != NULL)
	{
		line_len = new_line_pos - *saved + 1;
		*empty = ft_substr(*saved, 0, line_len);
		if (*empty == NULL)
			return (ft_freestr(saved), -1);
		return (save_in_static(saved, empty, new_line_pos, line_len));
	}
	if ((*saved)[0] == '\0')
		return (ft_freestr(saved), 0);
	*empty = *saved;
	*saved = NULL;
	return ((ssize_t)ft_strlen(*empty));
}

char	*read_until_eol_eof(int fd, char **saved, char **buffer)
{
	ssize_t		bytes_read;
	char		*temp;
	char		*compared;

	bytes_read = 1;
	compared = *saved;
	while (bytes_read > 0 && ft_strchr(compared, '\n') == NULL)
	{
		bytes_read = read(fd, *buffer, BUFFER_SIZE);
		if (bytes_read < 0)
			return (ft_freestr(saved), ft_freestr(buffer));
		if (bytes_read > 0)
		{
			(*buffer)[bytes_read] = '\0';
			compared = *buffer;
			temp = ft_strjoin(*saved, *buffer);
			if (temp == NULL)
				return (ft_freestr(saved), ft_freestr(buffer));
			ft_freestr(saved);
			*saved = temp;
		}
	}
	return (*saved);
}

ssize_t	get_next_line(int fd, char **empty)
{
	static char	*saved[MAX_FD];
	char		*buffer;
	ssize_t		line_len;

	if (fd < 0 || fd > MAX_FD || BUFFER_SIZE <= 0)
		return (-1);
	if (saved[fd] == NULL)
		saved[fd] = ft_strdup("");
	if (saved[fd] == NULL)
		return (-1);
	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (buffer == NULL)
		return (ft_freestr(&saved[fd]), -1);
	saved[fd] = read_until_eol_eof(fd, &saved[fd], &buffer);
	if (saved[fd] == NULL)
		return (ft_freestr(&buffer), -1);
	line_len = extract_line(&saved[fd], empty);
	ft_freestr(&buffer);
	return (line_len);
}
