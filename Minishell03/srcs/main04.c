/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main04.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 21:13:57 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 21:14:00 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	detect_heredoc(char *command, t_input_state *input_state)
{
	char	*ptr;

	ptr = command;
	while (*ptr != '\0')
	{
		if (my_strncmp(ptr, "<<", 2) == 0)
		{
			if (check_heredoc_delimiter(ptr, input_state))
			{
				if (is_heredoc_closed(ptr + 2 + ft_strlen \
				(input_state->heredoc_delim), input_state))
				{
					input_state->heredoc_active = 0;
					return (0);
				}
				input_state->heredoc_active = 1;
				return (1);
			}
		}
		ptr++;
	}
	return (0);
}

int	handle_input_loop(char **command, t_input_state *input_state)
{
	char	*more_input;

	while (input_state->heredoc_active)
	{
		more_input = readline("> ");
		if (!more_input)
		{
			free(*command);
			*command = NULL;
			return (1);
		}
		if (input_state->heredoc_delim && my_strncmp(input_state->heredoc_delim \
			, more_input, (size_t)ft_strlen(input_state->heredoc_delim)) == 0)
			input_state->heredoc_active = 0;
		check_quotes(*command, input_state);
		*command = combine_command(*command, more_input);
	}
	return (0);
}

int	process_input(char **command, t_input_state *input_state)
{
	input_state->heredoc_active = detect_heredoc(*command, input_state);
	check_quotes(*command, input_state);
	if (input_state->heredoc_active && is_heredoc_closed(*command, input_state))
	{
		input_state->heredoc_active = 0;
		return (0);
	}
	return (handle_input_loop(command, input_state));
}

void	free_command_and_delim(char **command, char **heredoc_delim)
{
	if (*command)
	{
		free(*command);
		*command = NULL;
	}
	if (*heredoc_delim)
	{
		free(*heredoc_delim);
		*heredoc_delim = NULL;
	}
}

int	handle_empty_command(char **command)
{
	if (command == NULL || *command == NULL)
		return (0);
	if ((*command)[0] == '\0')
	{
		free(*command);
		*command = NULL;
		return (1);
	}
	return (0);
}
