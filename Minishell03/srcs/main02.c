/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main02.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 21:12:08 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 21:12:12 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	reset_command_fields(t_command *cmd)
{
	reset_command_strings(cmd);
	cmd->prev_input_file = false;
	cmd->input_file = NULL;
	cmd->prev_output_file = false;
	cmd->output_file = NULL;
	cmd->prev_append_file = false;
	cmd->append_file = NULL;
	cmd->heredoc_content = NULL;
	cmd->heredoc_delim = NULL;
	cmd->close_heredoc_delim = NULL;
	cmd->input_fd = -1;
	cmd->output_fd = -1;
	cmd->pipe_fd[0] = -1;
	cmd->pipe_fd[1] = -1;
	cmd->executed_successfully = 0;
	cmd->is_heredoc_mark = false;
	cmd->prev_heredoc = false;
	cmd->next_heredoc = false;
}

void	reset_command_and_free_next(t_command *cmd)
{
	free_command_list_reset(cmd);
	reset_command_fields(cmd);
}

int	execute_and_reset(t_command *cmd, t_parse_context *context)
{
	int	status;

	if (!cmd)
		return (1);
	status = execute_command(cmd, context);
	reset_pipe_fds(cmd);
	reset_command_and_free_next(cmd);
	return (status);
}

char	*combine_command(char *command, char *more_input)
{
	char	*new_command;

	new_command = malloc(ft_strlen(command) + ft_strlen(more_input) + 2);
	if (!new_command)
	{
		perror("malloc");
		free(command);
		free(more_input);
		exit(EXIT_FAILURE);
	}
	ft_strcpy(new_command, command);
	my_strcat(new_command, "\n");
	my_strcat(new_command, more_input);
	free(command);
	free(more_input);
	return (new_command);
}

void	check_quotes(char *command, t_input_state *input_state)
{
	char	*ptr;

	ptr = command;
	while (*ptr != '\0')
	{
		if (*ptr == '\'')
			input_state->in_single_quotes = !input_state->in_single_quotes;
		else if (*ptr == '"')
			input_state->in_double_quotes = !input_state->in_double_quotes;
		ptr++;
	}
}
