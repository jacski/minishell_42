/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils02.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:00:34 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/06 10:00:38 by jkalinow         ###   ########.fr       */
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

void	free_command(t_command *cmd)
{
	t_command	*next_cmd;
	int			i;

	while (cmd)
	{
		next_cmd = cmd->next;
		i = 0;
		while (cmd->arguments[i])
		{
			free(cmd->arguments[i]);
			i++;
		}
		free(cmd->arguments);
		free(cmd->command);
		free(cmd);
		cmd = next_cmd;
	}
}
