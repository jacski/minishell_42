/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin01.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:28:13 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:28:15 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_input_redirection_builtin(t_command *cmd, int *saved_stdin)
{
	int	input_fd;

	if (cmd->input_file)
	{
		input_fd = open(cmd->input_file, O_RDONLY);
		if (input_fd < 0)
		{
			perror("open input file");
			return (-1);
		}
		*saved_stdin = dup(STDIN_FILENO);
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	return (0);
}

int	open_output_file(const char *file, int flags, const char *error_message)
{
	int	output_fd;

	output_fd = open(file, flags, 0644);
	if (output_fd < 0)
	{
		perror(error_message);
		return (-1);
	}
	return (output_fd);
}

void	redirect_output(int output_fd, int *saved_stdout)
{
	*saved_stdout = dup(STDOUT_FILENO);
	dup2(output_fd, STDOUT_FILENO);
	close(output_fd);
}

int	handle_output_redirection(t_command *cmd, int *saved_stdout)
{
	int	output_fd;

	if (cmd->output_file)
	{
		output_fd = open_output_file(cmd->output_file, O_WRONLY | O_CREAT | \
			O_TRUNC, "open output file");
		if (output_fd < 0)
			return (-1);
		redirect_output(output_fd, saved_stdout);
	}
	else if (cmd->append_file)
	{
		output_fd = open_output_file(cmd->append_file, O_WRONLY | O_CREAT | \
			O_APPEND, "open append file");
		if (output_fd < 0)
			return (-1);
		redirect_output(output_fd, saved_stdout);
	}
	return (0);
}

int	execute_cd(t_command *cmd)
{
	if (cmd->arguments[1] == NULL)
		chdir(getenv("HOME"));
	else
	{
		if (chdir(cmd->arguments[1]) != 0)
		{
			perror("cd");
			return (1);
		}
	}
	return (0);
}
