/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_strtok_quoted_strings.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/08 18:09:24 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/08 18:09:26 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*handle_quoted_strings(t_TokenizerState *state, const char *delim)
{
	char	*token;

	token = state->saveptr;
	while (*state->saveptr)
	{
		if (entering_quoted_string(state))
			continue ;
		if (exiting_quoted_string(state))
			continue ;
		if (handle_delimiters(state, delim))
			break ;
		state->saveptr++;
	}
	return (token);
}

bool	entering_quoted_string(t_TokenizerState *state)
{
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

bool	exiting_quoted_string(t_TokenizerState *state)
{
	if (*state->saveptr == state->quote_char && state->in_quotes)
	{
		state->in_quotes = false;
		state->quote_char = '\0';
		state->saveptr++;
		return (true);
	}
	return (false);
}

bool	handle_delimiters(t_TokenizerState *state, const char *delim)
{
	if (!state->in_quotes && strchr(delim, *state->saveptr))
	{
		*state->saveptr = '\0';
		state->saveptr++;
		return (true);
	}
	return (false);
}
