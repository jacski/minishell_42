/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_command01.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:40:16 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:40:19 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	check_command_null(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Command is NULL\n";
	if (cmd == NULL)
	{
		write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
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
			write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
			write(STDERR_FILENO, cmd->input_file, \
				ft_strlen(cmd->input_file));
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
		write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
		write(STDERR_FILENO, cmd->output_file, ft_strlen(cmd->output_file));
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
		write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
		write(STDERR_FILENO, cmd->append_file, ft_strlen(cmd->append_file));
		write(STDERR_FILENO, "\n", 1);
		return (-1);
	}
	return (0);
}

int	initialize_command(t_command *cmd)
{
	if (check_command_null(cmd) == -1)
		return (-1);
	if (handle_input_redirection_initialize_c(cmd) == -1)
		return (-1);
	if (cmd->output_file != NULL)
	{
		if (create_output_file(cmd) == -1)
			return (-1);
	}
	else if (cmd->append_file != NULL)
	{
		if (append_output_file(cmd) == -1)
			return (-1);
	}
	cmd->executed_successfully = 0;
	return (0);
}
