/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command02.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:54:07 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:54:10 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_input_redirection_exc(t_command *cmd)
{
	if (cmd->input_fd != -1)
	{
		dup2(cmd->input_fd, STDIN_FILENO);
		close(cmd->input_fd);
	}
}

void	handle_output_redirection_exc(t_command *cmd)
{
	if (cmd->output_fd != -1)
	{
		dup2(cmd->output_fd, STDOUT_FILENO);
		close(cmd->output_fd);
	}
}

void	handle_redirection_process(t_command *cmd)
{
	if (cmd->heredoc_delim != NULL && cmd->close_heredoc_delim != NULL && \
		ft_strcmp(cmd->heredoc_delim, cmd->close_heredoc_delim) == 0)
		handle_heredoc_exc(cmd);
	handle_input_redirection_exc(cmd);
	handle_output_redirection_exc(cmd);
}

void	execute_command_process(t_command *cmd)
{
	if (execvp(cmd->command, cmd->arguments) == -1)
	{
		write(STDERR_FILENO, "Error executing command: ", 25);
		write(STDERR_FILENO, cmd->command, ft_strlen(cmd->command));
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
