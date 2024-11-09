/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line02.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:12:23 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:12:27 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	**allocate_arguments(int max_args, t_command *cmd)
{
	int		i;

	cmd->arguments = malloc(sizeof(char *) * (max_args + 1));
	if (!cmd->arguments)
	{
		perror("malloc");
		return (NULL);
	}
	i = 0;
	while (i <= max_args)
		cmd->arguments[i++] = NULL;
	cmd->expand_flags = malloc((max_args + 1) * sizeof(bool));
	if (!cmd->expand_flags)
	{
		free(cmd->arguments);
		return (NULL);
	}
	i = 0;
	while (i <= max_args)
		cmd->expand_flags[i++] = false;
	return (cmd->arguments);
}

t_command	*create_command(t_command *cmd)
{
	init_command(cmd);
	cmd->arguments = allocate_arguments(128, cmd);
	if (!cmd->arguments)
	{
		free(cmd);
		exit(EXIT_FAILURE);
	}
	return (cmd);
}

void	handle_pipeline(t_command **current, int *arg_index)
{
	t_command	*new_cmd;

	new_cmd = malloc(sizeof(t_command));
	if (!new_cmd)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	init_command(new_cmd);
	new_cmd->arguments = allocate_arguments(128, new_cmd);
	(*current)->next = new_cmd;
	*current = new_cmd;
	*arg_index = 0;
}

void	handle_input_redirection(t_command *current, char *token, \
		t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (*token)
		current->prev_input_file = true;
}

void	handle_output_redirection_parsel(t_command *current, \
		char *token, t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (*token)
		current->prev_output_file = true;
}
