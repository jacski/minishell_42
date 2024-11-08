/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe02.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:38:01 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:38:04 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_and_exec_command(t_command *cmd, int in_fd, int pipe_fd[2])
{
	pid_t	pid;
	int		redir_status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
	{
		redir_status = setup_redirections(cmd, in_fd, pipe_fd);
		if (redir_status == -2)
			exit(127);
		if (redir_status == -1)
			exit(EXIT_FAILURE);
		if (execvp(cmd->arguments[0], cmd->arguments) == -1)
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	return (0);
}

int	create_pipe(int pipe_fd[2])
{
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (-1);
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
	if (handle_and_exec_command(cmd, *in_fd, pipe_fd) == -1)
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
	int			last_status;

	current_cmd = cmd;
	in_fd = 0;
	last_status = 0;
	while (current_cmd != NULL)
	{
		if (process_command(current_cmd, &in_fd) == -1)
			return (-1);
		current_cmd = current_cmd->next;
	}
	while (wait(&status) > 0)
		last_status = status;
	return ((last_status >> 8) & 0xFF);
}
