/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token04.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:07:45 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:07:48 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	handle_special_characters(t_parse_context *context, char **token)
{
	t_TokenizerState	*state;

	state = context->state;
	if (!state->in_quotes && ft_strchr("<>|", *state->saveptr))
	{
		if (*state->saveptr == '|')
		{
			*token = handle_pipe(context);
			return (true);
		}
		if (ft_strchr("<>", *state->saveptr) != NULL)
		{
			*token = handle_redirection_strtok(context, *token);
			return (true);
		}
	}
	return (false);
}

bool	has_unclosed_quote(t_TokenizerState *state)
{
	if (state->in_quotes && state->quote_char != '\0')
	{
		state->in_quotes = false;
		state->quote_char = '\0';
		return (true);
	}
	return (false);
}

char	*handle_quoted_strings(t_parse_context *context, const char *delim)
{
	char				*token;
	t_TokenizerState	*state;

	state = context->state;
	token = state->saveptr;
	while (*state->saveptr)
	{
		if (process_quoted_string(context))
			continue ;
		if (state->prev_heredoc)
			return (process_heredoc_content(context));
		if (state->is_heredoc_mark)
			return (process_heredoc_delimiter(context));
		if (handle_special_characters(context, &token))
			return (token);
		if (handle_delimiters(context, delim) && !state->prev_heredoc)
			break ;
		state->saveptr++;
	}
	has_unclosed_quote(state);
	return (finalize_token(context, token));
}

char	*my_strtok(t_parse_context *context, char *str, const char *delim)
{
	char				*token;
	t_TokenizerState	*state;

	state = context->state;
	if (str != NULL)
	{
		state->saveptr = str;
		state->in_quotes = false;
		state->quote_char = '\0';
		state->heredoc_delim[0] = '\0';
		state->is_heredoc_mark = false;
		state->prev_heredoc = false;
		state->in_single_quotes = false;
	}
	token = initialize_token(context, str, delim);
	if (!token)
		return (NULL);
	token = handle_quoted_strings(context, delim);
	if (!token)
		return (NULL);
	if (*state->saveptr == '\0')
		state->saveptr = NULL;
	if (ft_strcmp(token, "<<") == 0)
		state->saveptr += ft_strlen(token) + 1;
	return (token);
}
