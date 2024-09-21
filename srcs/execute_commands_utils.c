/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_commands_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:10:53 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 19:10:58 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	check_command(t_command *cmd)
{
	if (cmd == NULL)
	{
		write(STDERR_FILENO, "Command is NULL\n", 16);
		return (-1);
	}
	return (0);
}

pid_t	create_child_process(void)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		write(STDERR_FILENO, "Error creating child process\n", 29);
	}
	return (pid);
}

void	handle_redirection_process(t_command *cmd)
{
	if (cmd->input_fd != -1)
	{
		dup2(cmd->input_fd, STDIN_FILENO);
		close(cmd->input_fd);
	}
	if (cmd->output_fd != -1)
	{
		dup2(cmd->output_fd, STDOUT_FILENO);
		close(cmd->output_fd);
	}
}

void	execute_command_process(t_command *cmd)
{
	if (execvp(cmd->command, cmd->arguments) == -1)
	{
		write(STDERR_FILENO, "Error executing command: ", 25);
		write(STDERR_FILENO, cmd->command, strlen(cmd->command));
		write(STDERR_FILENO, "\n", 1);
		exit(EXIT_FAILURE);
	}
}

int	wait_for_child_process(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		write(STDERR_FILENO, "Error waiting for child process\n", 32);
		return (-1);
	}
	return (status);
}
