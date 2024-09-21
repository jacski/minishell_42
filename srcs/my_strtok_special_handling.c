/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_strtok_special_handling.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/08 17:26:23 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/08 17:26:25 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	handle_special_characters(t_TokenizerState *state, char **token)
{
	if (*state->saveptr == '|')
	{
		*token = handle_pipe(state);
		return (true);
	}
	if (strchr("<>", *state->saveptr) != NULL)
	{
		*token = handle_redirection_strtok(state, *token);
		return (true);
	}
	return (false);
}

char	*handle_pipe(t_TokenizerState *state)
{
	*state->saveptr = '\0';
	state->saveptr++;
	return (ft_strdup("|"));
}
