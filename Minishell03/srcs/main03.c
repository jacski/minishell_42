/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main03.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 21:13:05 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 21:13:08 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*extract_delimiter(char *ptr)
{
	char	*delim;
	char	*delim_end;

	delim = ft_strdup(ptr);
	delim_end = delim;
	while (*delim_end != '\0' && *delim_end != ' ' && *delim_end != '\n')
		delim_end++;
	*delim_end = '\0';
	return (delim);
}

int	check_line_match(char *line_start, char *line_end, t_input_state \
		*input_state)
{
	size_t	delim_len;

	delim_len = ft_strlen(input_state->heredoc_delim);
	if ((size_t)(line_end - line_start) == delim_len && \
	my_strncmp(line_start, input_state->heredoc_delim, \
	delim_len) == 0)
	{
		input_state->heredoc_active = 0;
		return (1);
	}
	return (0);
}

int	check_remaining_input(char *line_start, t_input_state *input_state)
{
	size_t	delim_len;
	char	*line_end;

	delim_len = ft_strlen(input_state->heredoc_delim);
	line_end = line_start + ft_strlen(line_start);
	if ((size_t)(line_end - line_start) == delim_len && \
	my_strncmp(line_start, input_state->heredoc_delim, \
	delim_len) == 0)
	{
		input_state->heredoc_active = 0;
		return (1);
	}
	return (0);
}

int	is_heredoc_closed(char *input, t_input_state *input_state)
{
	char	*line_start;
	char	*newline_pos;

	line_start = input;
	while (*line_start != '\0')
	{
		newline_pos = strchr(line_start, '\n');
		if (newline_pos == NULL)
			return (check_remaining_input(line_start, input_state));
		if (check_line_match(line_start, newline_pos, input_state))
			return (1);
		line_start = newline_pos + 1;
	}
	return (0);
}

int	check_heredoc_delimiter(char *ptr, t_input_state *input_state)
{
	ptr += 2;
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')
		ptr++;
	input_state->heredoc_delim = extract_delimiter(ptr);
	if (input_state->heredoc_delim[0] != '\0')
	{
		if (is_heredoc_closed(ptr + ft_strlen \
		(input_state->heredoc_delim), \
		input_state))
		{
			input_state->heredoc_active = 0;
			return (0);
		}
		input_state->heredoc_active = 1;
		return (1);
	}
	return (0);
}
