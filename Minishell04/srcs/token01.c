/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token01.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:01:10 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:01:13 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*initialize_token(t_parse_context *context, char *str, \
		const char *delim)
{
	t_TokenizerState	*state;

	state = context->state;
	if (str != NULL)
		state->saveptr = str;
	else if (state->saveptr == NULL)
		return (NULL);
	state->saveptr += my_strspn(state->saveptr, delim);
	if (*state->saveptr == '\0')
	{
		state->saveptr = NULL;
		return (NULL);
	}
	return (state->saveptr);
}

bool	entering_quoted_string(t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if ((*state->saveptr == '"' || *state->saveptr == '\'') \
		&& !state->in_quotes)
	{
		state->in_quotes = true;
		state->quote_char = *state->saveptr;
		state->saveptr++;
		return (true);
	}
	return (false);
}

bool	exiting_quoted_string(t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (*state->saveptr == state->quote_char && state->in_quotes)
	{
		state->in_quotes = false;
		state->quote_char = '\0';
		state->saveptr++;
		return (true);
	}
	return (false);
}

bool	handle_delimiters(t_parse_context *context, const char *delim)
{
	t_TokenizerState	*state;

	state = context->state;
	if (!state->in_quotes && ft_strchr(delim, *state->saveptr))
	{
		*state->saveptr = '\0';
		state->saveptr++;
		return (true);
	}
	return (false);
}

char	*handle_pipe(t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	*state->saveptr = '\0';
	state->saveptr++;
	return ("|");
}
