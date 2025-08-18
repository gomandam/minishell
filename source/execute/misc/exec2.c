/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/13 02:12:02 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

// recursive function for solving branches from AST

/*
 * binary operator node in the AST, connect left and right children
 * represents:
 * - priority binary nodes: AST_AND_IF, AST_OR_IF
 * - non-priority binary nodes: AST_PIPE
 */

/*
	Process Control:
	fork()
	execve()
	wait()
	waitpid()
	exit()
*/