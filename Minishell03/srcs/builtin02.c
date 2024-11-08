/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin02.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:29:19 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:29:21 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	execute_echo(t_command *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->arguments[1] && ft_strcmp(cmd->arguments[1], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (cmd->arguments[i])
	{
		write(1, cmd->arguments[i], ft_strlen(cmd->arguments[i]));
		if (cmd->arguments[i + 1])
		{
			write(1, " ", 1);
		}
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}

int	handle_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		write(STDOUT_FILENO, cwd, ft_strlen(cwd));
		write(STDOUT_FILENO, "\n", 1);
	}
	else
	{
		perror("getcwd");
		return (1);
	}
	return (0);
}

void	add_new_environment_variable(t_command *cmd, char *new_var, \
		int *var_count)
{
	char	**new_env;
	int		k;

	new_env = malloc((*var_count + 2) * sizeof(char *));
	if (!new_env)
	{
		perror("malloc");
		free(new_var);
		return ;
	}
	k = 0;
	while (k < *var_count)
	{
		new_env[k] = cmd->environment_variables[k];
		k++;
	}
	new_env[*var_count] = new_var;
	new_env[*var_count + 1] = NULL;
	free(cmd->environment_variables);
	cmd->environment_variables = new_env;
	(*var_count)++;
}

char	*create_new_variable(char *key, char *value)
{
	char	*new_var;

	new_var = malloc(ft_strlen(key) + ft_strlen(value) + 2);
	if (!new_var)
	{
		perror("malloc");
		return (NULL);
	}
	ft_strcpy(new_var, key);
	my_strcat(new_var, "=");
	my_strcat(new_var, value);
	return (new_var);
}

int	update_environment_variable(t_command *cmd, char *key, char *value, \
		int *var_count)
{
	int		j;
	char	*new_var;

	j = 0;
	while (j < *var_count)
	{
		if (my_strncmp(cmd->environment_variables[j], key, ft_strlen(key)) \
		== 0 && cmd->environment_variables[j][ft_strlen(key)] == '=')
		{
			free(cmd->environment_variables[j]);
			break ;
		}
		j++;
	}
	new_var = create_new_variable(key, value);
	if (!new_var)
		return (0);
	if (j < *var_count)
		cmd->environment_variables[j] = new_var;
	else
		add_new_environment_variable(cmd, new_var, var_count);
	return (1);
}
