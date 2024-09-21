/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 10:37:37 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 10:37:40 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_builtin(t_command *cmd)
{
	char	*builtins[8];
	int		i;

	builtins[0] = "cd";
	builtins[1] = "exit";
	builtins[2] = "echo";
	builtins[3] = "pwd";
	builtins[4] = "export";
	builtins[5] = "unset";
	builtins[6] = "env";
	builtins[7] = NULL;
	i = 0;
	while (builtins[i] != NULL)
	{
		if (strcmp(cmd->command, builtins[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

int	execute_builtin_command(t_command *cmd)
{
	if (strcmp(cmd->command, "cd") == 0)
		return (execute_cd(cmd));
	else if (strcmp(cmd->command, "exit") == 0)
		return (handle_exit());
	else if (strcmp(cmd->command, "echo") == 0)
		return (execute_echo(cmd));
	else if (strcmp(cmd->command, "pwd") == 0)
		return (handle_pwd());
	else if (strcmp(cmd->command, "export") == 0)
		return (handle_export(cmd));
	else if (strcmp(cmd->command, "unset") == 0)
		return (handle_unset(cmd));
	else if (strcmp(cmd->command, "env") == 0)
		return (handle_env());
	else
	{
		write(STDERR_FILENO, "Unknown built-in command: /n", 27);
		write(STDERR_FILENO, cmd->command, strlen(cmd->command));
		write(STDERR_FILENO, "\n", 1);
		return (1);
	}
}

void	restore_std(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}

int	handle_redirection_builtin(t_command *cmd, int *saved_stdin, \
		int *saved_stdout)
{
	if (handle_input_redirection_builtin(cmd, saved_stdin) < 0)
		return (1);
	if (handle_output_redirection(cmd, saved_stdout) < 0)
	{
		restore_std(*saved_stdin, -1);
		return (1);
	}
	return (0);
}

int	execute_builtin(t_command *cmd)
{
	int	saved_stdin;
	int	saved_stdout;
	int	result;

	saved_stdin = -1;
	saved_stdout = -1;
	if (handle_redirection_builtin(cmd, &saved_stdin, &saved_stdout) < 0)
		return (1);
	result = execute_builtin_command(cmd);
	restore_std(saved_stdin, saved_stdout);
	return (result);
}
