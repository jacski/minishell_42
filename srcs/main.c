/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:37:18 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 19:37:20 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

struct termios		g_orig_term;

int	is_empty_command(char *command)
{
	if (command[0] == '\0')
	{
		free(command);
		return (1);
	}
	return (0);
}

t_command	*parse_command(char *command)
{
	t_command	*cmd;

	cmd = parse_line(command);
	if (cmd == NULL)
	{
		write(STDERR_FILENO, "Failed to parse command\n", 24);
	}
	return (cmd);
}

int	execute_and_reset(t_command *cmd)
{
	int	status;

	status = execute_command(cmd);
	reset_pipe_fds(cmd);
	free_command(cmd);
	return (status);
}

void	execute_loop(void)
{
	char		*command;
	t_command	*cmd;
	int			status;

	cmd = NULL;
	while (1)
	{
		command = readline("minishell> ");
		if (handle_command_input(command))
			break ;
		if (is_empty_command(command))
			continue ;
		add_to_history(command);
		cmd = parse_command(command);
		free(command);
		if (cmd == NULL)
			continue ;
		status = execute_and_reset(cmd);
		if (status == -1)
			perror("execute_command");
	}
}

int	main(void)
{
	handle_signals();
	setup_terminal(&g_orig_term);
	execute_loop();
	return (0);
}
