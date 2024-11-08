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

void	handle_output_file(t_command *current, char *token)
{
	if (current->prev_output_file)
	{
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
		handle_output_file(cmd, token);
	else if (cmd->prev_append_file)
		handle_append_file(cmd, token);
	else
		handle_next_heredoc(cmd, token);
}

void	handle_heredoc_logic(t_command *cmd, char *token)
{
	if (cmd->prev_heredoc)
		handle_heredoc(cmd, token);
	else if (cmd->is_heredoc_mark)
	{
		cmd->heredoc_delim = ft_strdup(token);
		cmd->is_heredoc_mark = false;
		cmd->prev_heredoc = 1;
	}
}
