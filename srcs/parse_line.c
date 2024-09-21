/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 17:43:03 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/06 17:43:05 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_redirection_parse_l(t_command *current, char **token)
{
	if (strcmp(*token, "<") == 0)
	{
		*token = my_strtok(NULL, " \t\n");
		if (*token)
			current->input_file = strdup(*token);
	}
	else if (strcmp(*token, ">") == 0)
	{
		*token = my_strtok(NULL, " \t\n");
		if (*token)
			current->output_file = strdup(*token);
	}
	else if (strcmp(*token, ">>") == 0)
	{
		*token = my_strtok(NULL, " \t\n");
		if (*token)
			current->append_file = strdup(*token);
	}
}

void	handle_arguments(t_command *current, char *token, int *arg_index, \
		int max_args)
{
	char	*unquoted_token;

	unquoted_token = remove_quotes(token);
	if (*arg_index == 0)
		current->command = strdup(unquoted_token);
	if (*arg_index >= max_args - 1)
	{
		write(2, "Too many arguments for command: \n", 33);
		write(2, current->command, strlen(current->command));
		write(2, "\n", 1);
		return ;
	}
	current->arguments[(*arg_index)++] = strdup(unquoted_token);
}

void	process_tokens(t_command **head, t_command **current, char *line, \
		int max_args)
{
	char	*token;
	int		arg_index;

	token = my_strtok(line, " \t");
	arg_index = 0;
	while (token != NULL)
	{
		if (!*head)
		{
			*head = create_command(max_args);
			*current = *head;
		}
		if (strcmp(token, "|") == 0)
			handle_pipeline(current, &arg_index, max_args);
		else if (strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || \
			strcmp(token, ">>") == 0)
			handle_redirection_parse_l(*current, &token);
		else
			handle_arguments(*current, token, &arg_index, max_args);
		token = my_strtok(NULL, " \t");
	}
	if (*current)
		(*current)->arguments[arg_index] = NULL;
	expand_all_arguments(*head);
}

t_command	*parse_line(char *line)
{
	t_command	*head;
	t_command	*current;
	int			max_args;

	head = NULL;
	current = NULL;
	max_args = 20;
	process_tokens(&head, &current, line, max_args);
	return (head);
}
