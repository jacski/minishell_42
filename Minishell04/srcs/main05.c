/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main05.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 21:14:53 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 21:14:56 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_command_and_delim(char **command, char **heredoc_delim)
{
	if (*command)
	{
		free(*command);
		*command = NULL;
	}
	if (*heredoc_delim)
	{
		free(*heredoc_delim);
		*heredoc_delim = NULL;
	}
}

int	handle_empty_command(char **command)
{
	if (command == NULL || *command == NULL)
		return (0);
	if ((*command)[0] == '\0')
	{
		free(*command);
		*command = NULL;
		return (1);
	}
	return (0);
}

int	handle_command_inputs(char **command, t_input_state *input_state)
{
	if (process_input(command, input_state))
	{
		if (*command)
		{
			free(*command);
			*command = NULL;
		}
		return (1);
	}
	return (0);
}

int	process_input_and_expand(char **command, t_input_state *input_state, \
	t_command *cmd)
{
	if (handle_command_inputs(command, input_state))
		return (1);
	while (input_state->heredoc_active)
	{
		if (handle_command_inputs(command, input_state))
			return (1);
	}
	if (*command && !input_state->heredoc_active)
	{
		expand_dollar_question(*command, cmd);
		add_history(*command);
	}
	return (0);
}

int	parse_and_execute_command(char **command, t_command *cmd, \
		t_parse_context *context)
{
	context->arg_index = 0;
	cmd = parse_command(*command, cmd, context);
	if (!cmd)
	{
		free(*command);
		*command = NULL;
		return (1);
	}
	execute_and_reset(cmd, context);
	return (0);
}
