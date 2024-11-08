/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line05.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:16:32 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:16:35 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_argument_overflow_and_redirection(t_command *cmd, \
	t_parse_context *context, char *unquoted_token, char *token)
{
	if (context->arg_index >= context->max_args - 1)
	{
		handle_too_many_arguments(cmd);
		free(unquoted_token);
		return ;
	}
	if (cmd->prev_heredoc || cmd->is_heredoc_mark)
		handle_heredoc_logic(cmd, token);
	else
		handle_file_redirection_logic(cmd, unquoted_token, token, context);
}

void	handle_arguments(t_parse_context *context, char *token)
{
	char		*unquoted_token;
	t_command	*cmd;

	unquoted_token = remove_quotes(token);
	cmd = context->current;
	if (context->arg_index == 0)
	{
		if (cmd->command != NULL)
			free(cmd->command);
		cmd->command = ft_strdup(unquoted_token);
	}
	handle_argument_overflow_and_redirection(cmd, context, unquoted_token, \
		token);
	if (strchr(token, '$') && !(token[0] == '\'' && token[ft_strlen(token) \
		- 1] == '\''))
		cmd->expand_flags[context->arg_index] = true;
	else
		cmd->expand_flags[context->arg_index] = false;
	context->arg_index++;
	free(unquoted_token);
}

void	initialize_command_context(t_parse_context *context)
{
	if (!context->head)
	{
		context->head = malloc(sizeof(t_command));
		if (!context->head)
		{
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		init_command(context->head);
		context->head->arguments = allocate_arguments(128, context->head);
		if (!context->head->arguments)
		{
			free(context->head);
			exit(EXIT_FAILURE);
		}
		context->current = context->head;
		context->arg_index = 0;
	}
}

int	process_token(t_parse_context *context, char *token)
{
	initialize_command_context(context);
	if (token[0] == '\'' && token[ft_strlen(token) - 1] == '\'')
		context->state->in_single_quotes = true;
	else
		context->state->in_single_quotes = false;
	if (ft_strcmp(token, "|") == 0)
		handle_pipeline(&context->current, &context->arg_index);
	else if (ft_strcmp(token, "<") == 0 || ft_strcmp(token, ">") == 0 || \
		ft_strcmp(token, ">>") == 0 || ft_strcmp(token, "<<") == 0)
		handle_redirection_parse_l(context->current, token, context);
	else
		handle_arguments(context, token);
	return (0);
}

void	process_tokens(t_parse_context *context, char *line)
{
	char	*token;

	context->state->saveptr = line;
	token = my_strtok(context, line, " \t\n");
	while (token != NULL)
	{
		if (token[0] == '\0')
		{
			token = my_strtok(context, NULL, " \t\n");
			continue ;
		}
		if (process_token(context, token) == -1)
		{
			token = my_strtok(context, NULL, " \t\n");
			continue ;
		}
		token = my_strtok(context, NULL, " \t\n");
	}
	if (context->current)
		context->current->arguments[context->arg_index] = NULL;
	expand_all_arguments(context->head);
}
