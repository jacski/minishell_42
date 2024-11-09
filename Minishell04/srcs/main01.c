/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main01.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 21:11:09 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 21:11:12 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_command_input(char *command)
{
	if (command == NULL)
	{
		write(STDOUT_FILENO, "Exiting...\n", 11);
		return (1);
	}
	return (0);
}

void	reset_pipe_fds(t_command *cmd)
{
	t_command	*current_cmd;

	current_cmd = cmd;
	while (current_cmd)
	{
		current_cmd->pipe_fd[0] = -1;
		current_cmd->pipe_fd[1] = -1;
		current_cmd = current_cmd->next;
	}
}

t_command	*parse_command(char *command, t_command *cmd, \
			t_parse_context *context)
{
	cmd = parse_line(command, cmd, context);
	if (cmd == NULL)
	{
		write(STDERR_FILENO, "Failed to parse command\n", 24);
	}
	return (cmd);
}

void	free_command_list_reset(t_command *cmd)
{
	t_command	*current;
	t_command	*next;

	current = cmd->next;
	while (current != NULL)
	{
		next = current->next;
		free_command(current);
		free(current);
		current = next;
	}
	cmd->next = NULL;
}

void	reset_command_strings(t_command *cmd)
{
	if (cmd->command)
	{
		free(cmd->command);
		cmd->command = NULL;
	}
	if (cmd->arguments)
		free_arguments(cmd);
	free_heredoc(cmd);
	free_files(cmd);
}
