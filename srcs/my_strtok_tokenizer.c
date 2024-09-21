/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_strtok_tokenizer.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/08 17:23:13 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/08 17:23:17 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

size_t	my_strspn(const char *s1, const char *s2)
{
	size_t		count;
	const char	*p1;
	const char	*p2;

	count = 0;
	p1 = s1;
	while (*p1)
	{
		p2 = s2;
		while (*p2)
		{
			if (*p1 == *p2)
				break ;
			p2++;
		}
		if (*p2 == '\0')
			break ;
		count++;
		p1++;
	}
	return (count);
}

char	*initialize_token(t_TokenizerState *state, char *str, \
		const char *delim)
{
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

char	*my_strtok(char *str, const char *delim)
{
	static t_TokenizerState	state;
	char					*token;

	state.saveptr = NULL;
	state.in_quotes = false;
	state.quote_char = '\0';
	token = initialize_token(&state, str, delim);
	if (!token)
		return (NULL);
	token = handle_quoted_strings(&state, delim);
	if (!token)
		return (NULL);
	if (handle_special_characters(&state, &token))
		return (token);
	if (*state.saveptr == '\0')
		state.saveptr = NULL;
	return (ft_strdup(token));
}
