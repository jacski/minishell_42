/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line06.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:18:17 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:18:19 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_command	*parse_line(char *line, t_command *cmd, t_parse_context *context)
{
	t_TokenizerState	state;

	create_command(cmd);
	state.saveptr = NULL;
	state.in_quotes = false;
	state.in_single_quotes = false;
	state.quote_char = '\0';
	state.heredoc_delim[0] = '\0';
	state.prev_heredoc = false;
	context->head = cmd;
	context->current = cmd;
	context->state = &state;
	process_tokens(context, line);
	if (state.in_quotes)
		write(2, "Error: Unclosed quote at the end of input.\n", 43);
	return (context->head);
}
