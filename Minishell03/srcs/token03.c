/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token03.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:05:47 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:05:51 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	is_heredoc_delimiter(t_parse_context *context)
{
	size_t				heredoc_delim_len;
	char				*ptr;
	t_TokenizerState	*state;

	state = context->state;
	heredoc_delim_len = ft_strlen(state->heredoc_delim);
	ptr = state->saveptr;
	if (my_strncmp(ptr, state->heredoc_delim, heredoc_delim_len) == 0)
	{
		if (*(ptr + heredoc_delim_len) == '\n' || *(ptr + heredoc_delim_len) \
			== '\0')
		{
			ptr--;
			while (ptr > state->saveptr && ft_strchr(" \t", *ptr))
				ptr--;
			if (ptr == state->saveptr || *ptr == '\n')
				return (true);
		}
	}
	return (false);
}

char	*process_heredoc_delimiter(t_parse_context *context)
{
	t_TokenizerState	*state;
	int					i;

	state = context->state;
	while (*state->saveptr == ' ' || *state->saveptr == '\t')
		state->saveptr++;
	i = 0;
	while (!ft_strchr(" \t\n\0", state->saveptr[i]) && i < 255)
	{
		state->heredoc_delim[i] = state->saveptr[i];
		i++;
	}
	state->heredoc_delim[i] = '\0';
	state->saveptr += i + 1;
	state->is_heredoc_mark = false;
	state->prev_heredoc = true;
	return (state->heredoc_delim);
}

char	*process_heredoc_content(t_parse_context *context)
{
	char				*token;
	t_TokenizerState	*state;

	state = context->state;
	token = state->saveptr;
	while (*state->saveptr)
	{
		if (is_heredoc_delimiter(context))
		{
			if (*(state->saveptr - 1) == '\n')
				*(state->saveptr - 1) = '\0';
			state->prev_heredoc = false;
			break ;
		}
		state->saveptr++;
	}
	return (token);
}

bool	process_quoted_string(t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (entering_quoted_string(context))
		return (true);
	if (exiting_quoted_string(context))
		return (true);
	return (false);
}

char	*finalize_token(t_parse_context *context, char *token)
{
	t_TokenizerState	*state;

	state = context->state;
	if (state->saveptr == NULL)
		return (NULL);
	if (ft_strcmp(state->heredoc_delim, token) == 0)
		state->prev_heredoc = true;
	return (token);
}
