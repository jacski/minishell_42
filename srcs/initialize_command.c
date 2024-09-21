/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize_command.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 16:14:26 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/01 16:31:03 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	initialize_command(t_command *cmd)
{
	if (check_command_null(cmd) == -1)
		return (-1);
	if (handle_input_redirection_initialize_c(cmd) == -1)
		return (-1);
	if (cmd->output_file != NULL)
	{
		if (create_output_file(cmd) == -1)
			return (-1);
	}
	else if (cmd->append_file != NULL)
	{
		if (append_output_file(cmd) == -1)
			return (-1);
	}
	set_environment_variables(cmd);
	cmd->executed_successfully = 0;
	return (0);
}
