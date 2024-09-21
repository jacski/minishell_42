/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_commands.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 18:57:11 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 18:57:16 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	create_process(t_command *cmd)
{
	pid_t	pid;
	int		status;	

	if (check_command(cmd) == -1)
		return (-1);
	pid = create_child_process();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		handle_redirection_process(cmd);
		execute_command_process(cmd);
	}
	status = wait_for_child_process(pid);
	if (status == -1)
		return (-1);
	if (status == 0)
		cmd->executed_successfully = 1;
	else
		cmd->executed_successfully = 0;
	return (0);
}

int	execute_command(t_command *cmd)
{
	if (is_builtin(cmd))
		return (execute_builtin(cmd));
	else if (cmd->next)
	{
		if (setup_pipes(cmd) == -1)
		{
			write(STDERR_FILENO, "Error setting up pipes for pipeline\n", 36);
			return (-1);
		}
		return (execute_pipeline(cmd));
	}
	else
	{
		if (initialize_command(cmd) == -1)
		{
			write(STDERR_FILENO, "Error initializing command\n", 27);
			return (-1);
		}
		return (create_process(cmd));
	}
}
