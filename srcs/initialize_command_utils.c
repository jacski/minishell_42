/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize_command_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 21:11:29 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/04 21:11:35 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	check_command_null(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Command is NULL\n";
	if (cmd == NULL)
	{
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		return (-1);
	}
	return (0);
}

int	handle_input_redirection_initialize_c(t_command *cmd)
{
	const char	*error_msg = "Error opening input file: ";

	error_msg = "Error opening input file: ";
	if (cmd->input_file != NULL)
	{
		cmd->input_fd = open(cmd->input_file, O_RDONLY);
		if (cmd->input_fd == -1)
		{
			write(STDERR_FILENO, error_msg, strlen(error_msg));
			write(STDERR_FILENO, cmd->input_file, strlen(cmd->input_file));
			write(STDERR_FILENO, "\n", 1);
			return (-1);
		}
	}
	return (0);
}

int	create_output_file(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Error creating output file: ";
	cmd->output_fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (cmd->output_fd == -1)
	{
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		write(STDERR_FILENO, cmd->output_file, strlen(cmd->output_file));
		write(STDERR_FILENO, "\n", 1);
		return (-1);
	}
	return (0);
}

int	append_output_file(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Error appending to output file: ";
	cmd->output_fd = open(cmd->append_file, \
		O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (cmd->output_fd == -1)
	{
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		write(STDERR_FILENO, cmd->append_file, strlen(cmd->append_file));
		write(STDERR_FILENO, "\n", 1);
		return (-1);
	}
	return (0);
}

void	set_environment_variables(t_command *cmd)
{
	int	i;

	if (cmd->environment_variables != NULL)
	{
		i = 0;
		while (cmd->environment_variables[i])
		{
			putenv(cmd->environment_variables[i]);
			i++;
		}
	}
}
