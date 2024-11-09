/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path01.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:48:26 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:48:36 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*check_absolute_path(const char *cmd)
{
	if (!cmd)
		return (NULL);
	if (cmd[0] == '/')
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	return (NULL);
}

char	*get_env_variable_value(t_command *cmd, const char *var_name)
{
	int		i;
	size_t	name_len;

	if (!cmd || !cmd->environment_variables)
		return (NULL);
	name_len = ft_strlen((char *)var_name);
	i = 0;
	while (cmd->environment_variables[i] != NULL)
	{
		if (my_strncmp(cmd->environment_variables[i], var_name, name_len) \
		== 0 && cmd->environment_variables[i][name_len] == '=')
			return (cmd->environment_variables[i] + name_len + 1);
		i++;
	}
	return (NULL);
}

char	*duplicate_path_env(t_command *cmd)
{
	char	*path;

	path = get_env_variable_value(cmd, "PATH");
	if (!path || ft_strlen(path) == 0)
		return (NULL);
	return (ft_strdup(path));
}

char	*build_full_path(const char *dir, const char *cmd)
{
	char	*full_path;
	size_t	len;

	len = ft_strlen((char *)dir) + ft_strlen((char *)cmd) + 2;
	full_path = (char *)malloc(len);
	if (!full_path)
		return (NULL);
	ft_strcpy(full_path, (char *)dir);
	my_strcat(full_path, "/");
	my_strcat(full_path, cmd);
	return (full_path);
}

char	*iterate_directories(char *dir, const char *cmd)
{
	char	*full_path;

	while (dir != NULL)
	{
		full_path = build_full_path(dir, cmd);
		if (full_path && access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		dir = ft_strtok(NULL, ":");
	}
	return (NULL);
}
