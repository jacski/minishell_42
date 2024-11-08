/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe01.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:36:50 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:36:52 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	setup_input_redirection(t_command *cmd, int *in_fd)
{
	if (cmd->input_file != NULL)
	{
		*in_fd = open(cmd->input_file, O_RDONLY);
		if (*in_fd == -1)
		{
			write(STDERR_FILENO, "Error: Input file '", 19);
			write(STDERR_FILENO, cmd->input_file, strlen(cmd->input_file));
			write(STDERR_FILENO, "' could not be opened.\n", 24);
			return (-2);
		}
	}
	if (*in_fd != 0)
	{
		if (dup2(*in_fd, STDIN_FILENO) == -1)
		{
			write(STDERR_FILENO, "Error: Failed to redirect input.\n", 33);
			close(*in_fd);
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

int	setup_pipes(t_command *cmd)
{
	t_command	*current_cmd;

	if (cmd == NULL)
	{
		write(STDERR_FILENO, "Command is NULL\n", 16);
		return (-1);
	}
	current_cmd = cmd;
	while (current_cmd->next != NULL)
	{
		if (pipe(current_cmd->pipe_fd) == -1)
		{
			write(STDERR_FILENO, "Error creating pipe\n", 20);
			return (-1);
		}
		current_cmd->next->input_fd = current_cmd->pipe_fd[0];
		current_cmd = current_cmd->next;
	}
	return (0);
}

int	setup_redirections(t_command *cmd, int in_fd, int pipe_fd[2])
{
	int	redir_status;

	redir_status = setup_input_redirection(cmd, &in_fd);
	if (redir_status == -2)
		return (-2);
	if (redir_status == -1)
		return (-1);
	if (cmd->next != NULL)
	{
		if (setup_pipe_output(pipe_fd) == -1)
			return (-1);
	}
	else
	{
		if (setup_file_output(cmd) == -1)
			return (-1);
	}
	return (0);
}
