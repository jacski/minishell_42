/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 16:14:14 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 16:14:17 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	create_pipe(int pipe_fd[2])
{
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

void	exec_command(t_command *cmd)
{
	if (execvp(cmd->arguments[0], cmd->arguments) == -1)
	{
		perror("execvp");
		exit(EXIT_FAILURE);
	}
}

int	setup_input_redirection(t_command *cmd, int *in_fd)
{
	if (cmd->input_file != NULL)
	{
		*in_fd = open(cmd->input_file, O_RDONLY);
		if (*in_fd == -1)
		{
			perror("open input file");
			return (-1);
		}
	}
	if (*in_fd != 0)
	{
		if (dup2(*in_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 input");
			return (-1);
		}
		close(*in_fd);
	}
	return (0);
}

int	setup_pipe_output(int pipe_fd[2])
{
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		perror("dup2 output");
		return (-1);
	}
	close(pipe_fd[1]);
	close(pipe_fd[0]);
	return (0);
}

int	setup_file_output(t_command *cmd)
{
	int	out_fd;

	if (cmd->output_file != NULL)
	{
		out_fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1)
		{
			perror("open output file");
			return (-1);
		}
		if (dup2(out_fd, STDOUT_FILENO) == -1)
		{
			perror("dup2 output file");
			return (-1);
		}
		close(out_fd);
	}
	return (0);
}
