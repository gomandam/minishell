/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   literal.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migugar2 <migugar2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 12:55:32 by migugar2          #+#    #+#             */
/*   Updated: 2025/08/13 13:39:57 by migugar2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// TODO: For bash error oh ambiguos redirect (multiple args for only one redir, when expand wildcard)
char	*literal_expansion(t_tok *word)
{
	size_t		len;
	const char	*last_char;

	last_char = word->seg_tail->slice.begin + word->seg_tail->slice.len;
	len = last_char - word->seg_head->slice.begin;
	return (ft_strndup(word->seg_head->slice.begin, len));
}
