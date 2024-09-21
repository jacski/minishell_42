/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin_utils01.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 10:37:58 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/07 10:38:01 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
	{
		perror("getcwd");
		return (1);
	}
	return (0);
}

int	handle_export(t_command *cmd)
{
	int		i;
	char	*env;

	i = 1;
	while (cmd->arguments[i] != NULL)
	{
		env = strchr(cmd->arguments[i], '=');
		if (env)
		{
			*env = '\0';
			setenv(cmd->arguments[i], env + 1, 1);
		}
		else
			write(STDERR_FILENO, "Invalid format for export\n", 26);
		i++;
	}
	return (0);
}

int	handle_unset(t_command *cmd)
{
	int	i;

	i = 1;
	while (cmd->arguments[i] != NULL)
	{
		unsetenv(cmd->arguments[i]);
		i++;
	}
	return (0);
}

int	handle_env(void)
{
	int	i;

	i = 0;
	while (__environ[i] != NULL)
	{
		puts(__environ[i]);
		i++;
	}
	return (0);
}

int	handle_exit(void)
{
	write(1, "Exiting...\n", 12);
	exit(0);
}
