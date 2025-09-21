/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 02:04:11 by gomandam          #+#    #+#             */
/*   Updated: 2025/09/20 21:12:19 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

// Putting t_shell just in case, we need to use it. 
void	export_print_error(t_shell *shell, char *argv)
{
	(void)shell;
	ft_putstr_fd("minishell: export: '", 2);
	ft_putstr_fd(argv, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

/* Prints all exported vars: declare -x KEY="VALUE"
   For each node: if '=', print declare -x KEY="VALUE"; else declare -x KEY.
   Direct to STDOUT, traverses t_env *list	*/
int	export_print_all(t_shell *shell)
{
	t_env	*cur;
	const char	*eq;
	int		i;

	cur = shell->env_list.head;
	while (cur)
	{
		ft_putstr_fd("declare -x ", 1);
		eq = ft_strchr(cur->full, '=');
		if (eq)
		{
			i = 0;
			while (&cur->full[i] != eq)
				ft_putchar_fd(cur->full[i++], 1);
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd((char *)(eq + 1), 1);
			ft_putstr_fd("\"\n", 1);
		}
		else
		{
			ft_putstr_fd(cur->full, 1);
			ft_putchar_fd('\n', 1);
		}
		cur = cur->next;
	}
	return (0);
}
