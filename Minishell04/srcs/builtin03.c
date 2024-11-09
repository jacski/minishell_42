/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin03.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:30:44 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:30:47 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	process_export_argument(t_command *cmd, char *arg, int *var_count)
{
	char	*env;

	env = ft_strchr(arg, '=');
	if (env)
	{
		*env = '\0';
		return (update_environment_variable(cmd, arg, env + 1, var_count));
	}
	return (0);
}

int	count_environment_variables(t_command *cmd)
{
	int	count;

	count = 0;
	while (cmd->environment_variables[count] != NULL)
		count++;
	return (count);
}

int	handle_export(t_command *cmd)
{
	int	var_count;
	int	i;

	var_count = count_environment_variables(cmd);
	i = 1;
	while (cmd->arguments[i] != NULL)
	{
		if (!process_export_argument(cmd, cmd->arguments[i], &var_count))
			write(STDERR_FILENO, "Invalid format for export\n", 26);
		i++;
	}
	return (0);
}

void	shift_environment_variables(t_command *cmd, int start_index)
{
	int	j;

	j = start_index;
	while (cmd->environment_variables[j + 1] != NULL)
	{
		cmd->environment_variables[j] = cmd->environment_variables[j + 1];
		j++;
	}
	cmd->environment_variables[j] = NULL;
}

void	unset_variable(t_command *cmd, char *argument, int env_count)
{
	int	j;
	int	var_len;

	j = 0;
	var_len = ft_strlen(argument);
	while (cmd->environment_variables[j] != NULL)
	{
		if (my_strncmp(cmd->environment_variables[j], argument, var_len) \
		== 0 && cmd->environment_variables[j][var_len] == '=')
		{
			free(cmd->environment_variables[j]);
			shift_environment_variables(cmd, j);
			env_count--;
			break ;
		}
		j++;
	}
}
