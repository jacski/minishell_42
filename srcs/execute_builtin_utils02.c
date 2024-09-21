/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin_utils02.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 10:38:10 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 10:38:13 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

int	execute_echo(t_command *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->arguments[1] && strcmp(cmd->arguments[1], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (cmd->arguments[i])
	{
		write(1, cmd->arguments[i], strlen(cmd->arguments[i]));
		if (cmd->arguments[i + 1])
		{
			write(1, " ", 1);
		}
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}
