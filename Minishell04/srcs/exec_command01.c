/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command01.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:52:44 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:52:46 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	process_dollar_question_replacement(char *pos, char *buffer, \
		size_t buffer_len)
{
	size_t	src_len;
	size_t	remaining_size;

	src_len = ft_strlen(pos + 2) + 1;
	remaining_size = (char *)pos + buffer_len + src_len - pos;
	if ((buffer_len + src_len) > remaining_size)
	{
		write(STDERR_FILENO, "Error: insufficient space \
			in destination buffer\n", 48);
		return ;
	}
	my_memmove(pos + buffer_len, pos + 2, src_len, remaining_size);
	my_memcpy(pos, buffer, buffer_len);
}

void	expand_dollar_question(char *input, t_command *cmd)
{
	char	buffer[12];
	char	*pos;
	size_t	buffer_len;

	pos = input;
	my_itoa(cmd->exit_status, buffer);
	buffer_len = ft_strlen(buffer);
	pos = my_strstr(pos, "$?");
	while (pos != NULL)
	{
		process_dollar_question_replacement(pos, buffer, buffer_len);
		pos = my_strstr(pos + buffer_len, "$?");
	}
}

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

void	handle_heredoc_exc(t_command *cmd)
{
	int	heredoc_pipe[2];

	if (pipe(heredoc_pipe) == -1)
	{
		write(STDERR_FILENO, "Error creating heredoc pipe\n", 28);
		exit(EXIT_FAILURE);
	}
	write(heredoc_pipe[1], cmd->heredoc_content, ft_strlen \
		(cmd->heredoc_content));
	write(heredoc_pipe[1], "\n", 1);
	close(heredoc_pipe[1]);
	dup2(heredoc_pipe[0], STDIN_FILENO);
	close(heredoc_pipe[0]);
}
