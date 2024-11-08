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

void	handle_append_redirection(t_command *current, char *token, \
		t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (*token)
		current->prev_append_file = true;
}
