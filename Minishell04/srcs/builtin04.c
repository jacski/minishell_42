/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin04.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:33:26 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:33:28 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_unset(t_command *cmd)
{
	int	i;
	int	env_count;

	i = 1;
	env_count = count_environment_variables(cmd);
	while (cmd->arguments[i] != NULL)
	{
		unset_variable(cmd, cmd->arguments[i], env_count);
		i++;
	}
	return (0);
}

int	handle_env(t_command *cmd)
{
	int		i;
	size_t	len;

	if (!cmd || !cmd->environment_variables)
		return (1);
	i = 0;
	while (cmd->environment_variables[i] != NULL)
	{
		len = strlen(cmd->environment_variables[i]);
		write(STDOUT_FILENO, cmd->environment_variables[i], len);
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
	return (0);
}

int	handle_exit(t_command *cmd, t_parse_context *context)
{
	free_environment_variables(cmd);
	free_command_list(cmd);
	free(context);
	write(1, "Exiting...\n", 12);
	exit(0);
}

int	is_builtin(t_command *cmd)
{
	char	*builtins[8];
	int		i;

	if (cmd->command == NULL)
		return (0);
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
		if (ft_strcmp(cmd->command, builtins[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

int	execute_builtin_command(t_command *cmd, t_parse_context *context)
{
	if (ft_strcmp(cmd->command, "cd") == 0)
		return (execute_cd(cmd));
	else if (ft_strcmp(cmd->command, "exit") == 0)
		return (handle_exit(cmd, context));
	else if (ft_strcmp(cmd->command, "echo") == 0)
		return (execute_echo(cmd));
	else if (ft_strcmp(cmd->command, "pwd") == 0)
		return (handle_pwd());
	else if (ft_strcmp(cmd->command, "export") == 0)
		return (handle_export(cmd));
	else if (ft_strcmp(cmd->command, "unset") == 0)
		return (handle_unset(cmd));
	else if (ft_strcmp(cmd->command, "env") == 0)
		return (handle_env(cmd));
	else
	{
		write(STDERR_FILENO, "Unknown built-in command: /n", 27);
		write(STDERR_FILENO, cmd->command, ft_strlen(cmd->command));
		write(STDERR_FILENO, "\n", 1);
		return (1);
	}
}
