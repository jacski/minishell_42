/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 14:43:28 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 14:43:31 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
	if (setup_input_redirection(cmd, &in_fd) == -1)
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

int	handle_fork(t_command *cmd, int in_fd, int pipe_fd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
	{
		if (setup_redirections(cmd, in_fd, pipe_fd) == -1)
			exit(EXIT_FAILURE);
		exec_command(cmd);
	}
	return (0);
}

int	process_command(t_command *cmd, int *in_fd)
{
	int	pipe_fd[2];

	if (cmd->next != NULL)
	{
		if (create_pipe(pipe_fd) == -1)
			return (-1);
	}
	if (handle_fork(cmd, *in_fd, pipe_fd) == -1)
		return (-1);
	if (cmd->next != NULL)
		close(pipe_fd[1]);
	if (*in_fd != 0)
		close(*in_fd);
	if (cmd->next != NULL)
		*in_fd = pipe_fd[0];
	else
		*in_fd = 0;
	return (0);
}

int	execute_pipeline(t_command *cmd)
{
	t_command	*current_cmd;
	int			in_fd;
	int			status;

	current_cmd = cmd;
	in_fd = 0;
	while (current_cmd != NULL)
	{
		if (process_command(current_cmd, &in_fd) == -1)
			return (-1);
		current_cmd = current_cmd->next;
	}
	while (wait(&status) > 0)
		;
	return (0);
}
