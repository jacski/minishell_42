/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token02.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:02:50 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:03:38 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*handle_non_token_redirection(t_parse_context *context, char *token)
{
	int					len;
	char				*separ_spec_char;
	char				*result;
	t_TokenizerState	*state;

	state = context->state;
	len = state->saveptr - token;
	separ_spec_char = (char *)malloc((len + 1) * sizeof(char));
	if (separ_spec_char == NULL)
	{
		perror("Failed to allocate memory");
		return (NULL);
	}
	my_strncpy(separ_spec_char, token, len);
	separ_spec_char[len] = '\0';
	result = ft_strdup(separ_spec_char);
	free(separ_spec_char);
	return (result);
}

char	*handle_double_redirection(t_parse_context *context)
{
	char				*saveptr_backup;
	int					i;
	t_TokenizerState	*state;

	state = context->state;
	if (*state->saveptr == '<' && *(state->saveptr + 1) == '<')
	{
		saveptr_backup = state->saveptr;
		state->saveptr += 2;
		while (*state->saveptr == ' ' || *state->saveptr == '\t')
			state->saveptr++;
		i = 0;
		while (!ft_strchr(" \t\n\0", state->saveptr[i]))
			i++;
		my_strncpy(state->heredoc_delim, state->saveptr, i);
		state->heredoc_delim[i] = '\0';
		state->saveptr = saveptr_backup;
		state->is_heredoc_mark = true;
		return ("<<\0");
	}
	state->saveptr += 2;
	return (">>");
}

char	*handle_single_redirection(t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (*state->saveptr == '<')
	{
		state->saveptr++;
		return ("<");
	}
	state->saveptr++;
	return (">");
}

char	*handle_specific_redirection(t_parse_context *context)
{
	t_TokenizerState	*state;

	state = context->state;
	if (((*state->saveptr == '>' && *(state->saveptr + 1) == '>') || \
		(*state->saveptr == '<' && *(state->saveptr + 1) == '<')))
		return (handle_double_redirection(context));
	else
		return (handle_single_redirection(context));
}

char	*handle_redirection_strtok(t_parse_context *context, char *token)
{
	char				*result;
	t_TokenizerState	*state;

	state = context->state;
	if (state->saveptr != token)
	{
		result = handle_non_token_redirection(context, token);
		if (result)
			return (result);
	}
	else
		return (handle_specific_redirection(context));
	return (NULL);
}
