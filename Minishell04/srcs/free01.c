/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free01.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:22:44 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:22:47 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_command_fields(t_command *command)
{
	if (command->command)
	{
		free(command->command);
		command->command = NULL;
	}
}

void	free_arguments(t_command *command)
{
	int	i;

	if (command->arguments)
	{
		i = 0;
		while (i <= 128 && command->arguments[i] != NULL)
		{
			free(command->arguments[i]);
			command->arguments[i] = NULL;
			i++;
		}
		if (command->expand_flags)
		{
			free(command->expand_flags);
			command->expand_flags = NULL;
		}
		free(command->arguments);
		command->arguments = NULL;
	}
}

void	free_files(t_command *command)
{
	if (command->input_file)
	{
		free(command->input_file);
		command->input_file = NULL;
	}
	if (command->output_file)
	{
		free(command->output_file);
		command->output_file = NULL;
	}
	if (command->append_file)
	{
		free(command->append_file);
		command->append_file = NULL;
	}
}

void	free_heredoc(t_command *command)
{
	if (command->heredoc_delim)
	{
		free(command->heredoc_delim);
		command->heredoc_delim = NULL;
	}
	if (command->close_heredoc_delim)
	{
		free(command->close_heredoc_delim);
		command->close_heredoc_delim = NULL;
	}
	if (command->heredoc_content)
	{
		free(command->heredoc_content);
		command->heredoc_content = NULL;
	}
}

void	free_environment_variables(t_command *command)
{
	int	i;

	if (command->environment_variables)
	{
		i = 0;
		while (command->environment_variables[i] != NULL)
		{
			free(command->environment_variables[i]);
			command->environment_variables[i] = NULL;
			i++;
		}
		free(command->environment_variables);
		command->environment_variables = NULL;
	}
}
