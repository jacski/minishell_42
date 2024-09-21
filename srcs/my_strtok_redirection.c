/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_strtok_redirection.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/08 17:45:01 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/08 17:45:05 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*handle_redirection_strtok(t_TokenizerState *state, char *token)
{
	char	*result;

	if (state->saveptr != token)
	{
		result = handle_non_token_redirection(state, token);
		if (result)
			return (result);
	}
	else
		return (handle_specific_redirection(state));
	return (NULL);
}

char	*handle_non_token_redirection(t_TokenizerState *state, char *token)
{
	int		len;
	char	*separ_spec_char;

	len = state->saveptr - token;
	separ_spec_char = (char *)malloc((len + 1) * sizeof(char));
	if (separ_spec_char == NULL)
	{
		perror("Failed to allocate memory");
		return (NULL);
	}
	strncpy(separ_spec_char, token, len);
	separ_spec_char[len] = '\0';
	return (ft_strdup(separ_spec_char));
}

char	*handle_specific_redirection(t_TokenizerState *state)
{
	if (((*state->saveptr == '>' && *(state->saveptr + 1) == '>') || \
		(*state->saveptr == '<' && *(state->saveptr + 1) == '<')))
		return (handle_double_redirection(state));
	else
		return (handle_single_redirection(state));
}

char	*handle_double_redirection(t_TokenizerState *state)
{
	char	*separ_spec_char;

	separ_spec_char = (char *)malloc(3 * sizeof(char));
	if (separ_spec_char == NULL)
	{
		perror("Failed to allocate memory");
		return (NULL);
	}
	separ_spec_char[0] = *state->saveptr;
	separ_spec_char[1] = *(state->saveptr + 1);
	separ_spec_char[2] = '\0';
	state->saveptr += 2;
	return (ft_strdup(separ_spec_char));
}

char	*handle_single_redirection(t_TokenizerState *state)
{
	char	*separ_spec_char ;

	separ_spec_char = (char *)malloc(2 * sizeof(char));
	if (separ_spec_char == NULL)
	{
		perror("Failed to allocate memory");
		return (NULL);
	}
	separ_spec_char[0] = *state->saveptr;
	separ_spec_char[1] = '\0';
	state->saveptr++;
	return (ft_strdup(separ_spec_char));
}
