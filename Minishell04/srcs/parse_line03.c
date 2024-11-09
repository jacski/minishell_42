/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line03.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:14:07 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:14:11 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_append_redirection(t_command *current, char *token, \
		t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (*token)
		current->prev_append_file = true;
}

void	handle_heredoc_redirection(t_command *current, char *token, \
		t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (*token)
		current->is_heredoc_mark = true;
}

void	handle_redirection_parse_l(t_command *current, char *token, \
		t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (ft_strcmp(token, ">>") == 0)
		handle_append_redirection(current, token, context);
	else if (ft_strcmp(token, "<<") == 0)
		handle_heredoc_redirection(current, token, context);
	else if (ft_strcmp(token, "<") == 0)
		handle_input_redirection(current, token, context);
	else if (ft_strcmp(token, ">") == 0)
		handle_output_redirection_parsel(current, token, context);
}

void	handle_too_many_arguments(t_command *current)
{
	write(2, "Too many arguments for command: \n", 33);
	write(2, current->command, ft_strlen(current->command));
	write(2, "\n", 1);
}

void	handle_heredoc(t_command *current, char *token)
{
	if (current->heredoc_delim && current->heredoc_content == NULL)
	{
		if (token != NULL)
		{
			current->heredoc_content = ft_strdup(token);
			current->prev_heredoc = false;
		}
		current->next_heredoc = true;
	}
	if (current->heredoc_delim == NULL)
		current->heredoc_delim = ft_strdup(token);
}
