/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin_utils03.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 13:01:56 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 13:02:02 by jkalinow         ###   ########.fr       */
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
