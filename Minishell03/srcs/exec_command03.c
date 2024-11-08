/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command03.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:55:11 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:55:13 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	fork_and_exec(t_command *cmd, char *command_path)
{
	extern char	**environ;
	pid_t		pid;

	pid = fork();
	if (pid == -1)
	{
		free(command_path);
		return (-1);
	}
	if (pid == 0)
	{
		handle_redirection_process(cmd);
		if (execve(command_path, cmd->arguments, environ) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
	}
	return (pid);
}

int	handle_process_exit_status(int status, t_command *cmd)
{
	if ((status & 0x7F) == 0)
		cmd->exit_status = (status >> 8) & 0xFF;
	else if ((status & 0x7F) != 0)
		cmd->exit_status = 128 + (status & 0x7F);
	else
		cmd->exit_status = (-1);
	if (status == 0)
		cmd->executed_successfully = 1;
	else
		cmd->executed_successfully = 0;
	return (0);
}

int	create_process(t_command *cmd)
{
	char	*command_path;
	int		pid;
	int		status;

	if (!cmd)
		return (-1);
	command_path = get_command_path(cmd);
	if (!command_path)
		return (-1);
	pid = fork_and_exec(cmd, command_path);
	if (pid == -1)
	{
		free(command_path);
		return (-1);
	}
	if (waitpid(pid, &status, 0) == -1)
	{
		free(command_path);
		return (-1);
	}
	free(command_path);
	return (handle_process_exit_status(status, cmd));
}

int	handle_regular_command(t_command *cmd)
{
	if (cmd->next)
	{
		if (setup_pipes(cmd) == -1)
		{
			write(STDERR_FILENO, "Error setting up pipes for pipeline\n", 36);
			return (-1);
		}
		return (execute_pipeline(cmd));
	}
	else
	{
		if (initialize_command(cmd) == -1)
		{
			write(STDERR_FILENO, "Error initializing command\n", 27);
			return (-1);
		}
		return (create_process(cmd));
	}
}

int	execute_command(t_command *cmd, t_parse_context *context)
{
	int	result;

	if (is_builtin(cmd))
		result = execute_builtin(cmd, context);
	else
		result = handle_regular_command(cmd);
	return (result);
}
