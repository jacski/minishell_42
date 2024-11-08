/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path02.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:50:09 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:50:11 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*find_in_directories(char *paths, const char *cmd)
{
	char	*paths_copy;
	char	*dir;
	char	*full_path;

	paths_copy = ft_strdup(paths);
	if (!paths_copy)
		return (NULL);
	dir = ft_strtok(paths_copy, ":");
	full_path = iterate_directories(dir, cmd);
	free(paths_copy);
	return (full_path);
}

char	*find_command_in_path(const char *cmd, t_command *command_struct)
{
	char	*command;
	char	*paths;
	char	*paths_copy;

	if (!cmd || !command_struct)
		return (NULL);
	command = check_absolute_path(cmd);
	if (command)
		return (command);
	paths = duplicate_path_env(command_struct);
	if (!paths)
		return (NULL);
	paths_copy = ft_strdup(paths);
	free(paths);
	if (!paths_copy)
		return (NULL);
	command = find_in_directories(paths_copy, cmd);
	free(paths_copy);
	return (command);
}

char	*get_command_path(t_command *cmd)
{
	char	*command_path;

	if (!cmd || !cmd->command)
	{
		write(STDERR_FILENO, "Command not found or is NULL\n", 29);
		if (cmd)
			cmd->exit_status = 127;
		return (NULL);
	}
	command_path = find_command_in_path(cmd->command, cmd);
	if (!command_path)
	{
		write(STDERR_FILENO, "Command not found or PATH is not set\n", 37);
		cmd->exit_status = 127;
	}
	return (command_path);
}
