/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gomandam <gomandam@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 00:24:27 by gomandam          #+#    #+#             */
/*   Updated: 2025/08/15 02:00:07 by gomandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// MAIN EXEC. FUNCTION -> Prototype: pipe, execution, process control, 
// 	recursive functions to traverse AST

// #include "../../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// TESTING behaviours
typedef enum e_asttype
{
    AST_CMD,
    AST_PIPE,
    AST_AND_IF,
    AST_OR_IF,
    AST_SUBSH
}  t_asttype;

typedef struct s_ast t_ast;

typedef struct s_cmd
{
    char **argv;
}	t_cmd;

typedef struct s_op
{
    t_ast *left;
    t_ast *right;
}   t_op;

typedef struct s_subsh
{
    t_ast *child;
}   t_subsh;

struct s_ast {
    t_asttype type;
    union {
        t_cmd cmd;
        t_op op;
        t_subsh subsh;
    } u_data;
};

//Prototyped for definition
int	exec_pipe_node(t_ast *left, t_ast *right);

int	exec_cmd_node(char *argv[])
{
	pid_t	pid;
	int	status;

	if (!argv || !argv[0])
		return (1);
	pid = fork();
	if (pid == -1)
		return(perror("fork"), 1);
	if (pid == 0)
	{
		if (execve(argv[0], argv, NULL) == -1)	// child process
		{
			perror("execve");
			exit(127); // command not found
		}
	}
	else
		waitpid(pid, &status, 0); // parent process
	return (status);
}

// recursive
int	exec_ast(t_ast *node)
{
	int	s;
	if (!node)
		return (1);
	if (node->type == AST_CMD)
		return exec_cmd_node(node->u_data.cmd.argv);
	if (node->type == AST_PIPE)
		return exec_pipe_node(node->u_data.op.left, node->u_data.op.right);
	if (node->type == AST_AND_IF)
	{
		s = exec_ast(node->u_data.op.left);
		if (WIFEXITED(s) && WEXITSTATUS(s) == 0)
			return exec_ast(node->u_data.op.right);
		return (s);
	}
	if (node->type == AST_OR_IF)
	{
		s = exec_ast(node->u_data.op.left);
		if (WIFEXITED(s) && WEXITSTATUS(s) != 0)
			return exec_ast(node->u_data.op.right);
		return s;
	}
	if (node->type == AST_SUBSH)
	{
		pid_t	pid;
		pid = fork();
		int	status;

		status = 0;
		if (pid == 0)
			exit(exec_ast(node->u_data.subsh.child));
		else if (pid > 0)
			waitpid(pid, &status, 0);
		else
			perror("fork");
		return (status);
	}
	return (1);
}
// Utility for pipes (left | right)
int	exec_pipe_node(t_ast *left, t_ast *right)
{
	int	fd[2];
	pid_t	pid_left;
	pid_t	pid_right;
	int	status;

	status = 0;
	if (pipe(fd) == -1)
		return (perror("pipe"), 1);
	pid_left = fork();
	if (pid_left == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]); close(fd[1]);
		exit(exec_ast(left));
	}
	pid_right = fork();
	if (pid_right == 0)
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]); close(fd[1]);
		exit(exec_ast(right));
	}
	close(fd[0]); close(fd[1]);
	waitpid(pid_left, &status, 0);
	waitpid(pid_right, &status, 0);
	return status;
}

// Debug: build a small AST for testing
t_ast	*build_test_ast(void)
{
	// Prepare command: /bin/echo hello
	t_ast *echo_node = malloc(sizeof(t_ast));
	echo_node->type = AST_CMD;
	char **argv = malloc(sizeof(char*) * 3);
	argv[0] = "/bin/echo";
	argv[1] = "hello";
	argv[2] = NULL;
	echo_node->u_data.cmd.argv = argv;
	// Prepare command: /bin/ls -l
	t_ast *ls_node = malloc(sizeof(t_ast));
	ls_node->type = AST_CMD;
	char **argv2 = malloc(sizeof(char*) * 3);
	argv2[0] = "/bin/ls";
	argv2[1] = "-l";
	argv2[2] = NULL;
	ls_node->u_data.cmd.argv = argv2;
	// Pipe: echo | ls -l
	t_ast *pipe_node = malloc(sizeof(t_ast));
	pipe_node->type = AST_PIPE;
	pipe_node->u_data.op.left = echo_node;
	pipe_node->u_data.op.right = ls_node;
	return pipe_node;
}

void	free_ast(t_ast *node)
{
	if (!node)
		return;
	if (node->type == AST_CMD)
		free(node->u_data.cmd.argv); // argv is mallocd
	else if (node->type == AST_PIPE || node->type == AST_AND_IF
			|| node->type == AST_OR_IF)
	{
		free_ast(node->u_data.op.left);
		free_ast(node->u_data.op.right);
	}
	else if (node->type == AST_SUBSH)
		free_ast(node->u_data.subsh.child);
	free(node);
}

// Main TEST-debug
int	main(void)
{
	t_ast *root = build_test_ast();
	printf("Executing AST...\n");
	int status = exec_ast(root);
	free_ast(root);
	printf("Done, status=%d\n", WEXITSTATUS(status));
	return (0);
}
