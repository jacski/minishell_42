/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line04.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:15:14 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:15:17 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_input_file(t_command *current, char *token)
{
	if (current->prev_input_file)
	{
		if (current->input_file)
		{
			free(current->input_file);
			current->input_file = NULL;
		}
		if (token != NULL)
		{
			current->input_file = ft_strdup(token);
			current->prev_input_file = false;
		}
	}
}

void	handle_output_file(t_command *current, char *token)
{
	if (current->prev_output_file)
	{
		if (current->output_file)
		{
			free(current->output_file);
			current->output_file = NULL;
		}
		if (token != NULL)
		{
			current->output_file = ft_strdup(token);
			current->prev_output_file = false;
		}
	}
}

void	handle_append_file(t_command *current, char *token)
{
	if (current->prev_append_file)
	{
		if (current->append_file)
		{
			free(current->append_file);
			current->append_file = NULL;
		}
		if (token != NULL)
		{
			current->append_file = ft_strdup(token);
			current->prev_append_file = false;
		}
	}
}

void	handle_next_heredoc(t_command *current, char *token)
{
	if (token != NULL)
	{
		if (current->close_heredoc_delim != NULL)
			free(current->close_heredoc_delim);
		current->close_heredoc_delim = ft_strdup(token);
		current->next_heredoc = false;
	}
}

void	handle_file_redirection_logic(t_command *cmd, char *unquoted_token, \
		char *token, t_parse_context *context)
{
	if (!cmd->next_heredoc && !cmd->prev_input_file && \
	!cmd->prev_output_file && !cmd->prev_append_file)
	{
		if (cmd->arguments[context->arg_index] != NULL)
			free(cmd->arguments[context->arg_index]);
		cmd->arguments[context->arg_index] = ft_strdup(unquoted_token);
	}
	else if (cmd->prev_input_file)
		handle_input_file(cmd, token);
	else if (cmd->prev_output_file)
	{
		handle_output_file(cmd, token);
		context->arg_index--;
	}
	else if (cmd->prev_append_file)
		handle_append_file(cmd, token);
	else
	{
		if (!cmd->has_heredoc)
		{
			handle_next_heredoc(cmd, token);
			cmd->has_heredoc = true;
		}
	}
}
