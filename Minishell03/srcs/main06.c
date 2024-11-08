/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main06.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 21:16:04 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 21:16:07 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

struct termios		g_orig_term;

void	copy_environment_variables(t_command *cmd)
{
	int	i;
	int	env_count;

	env_count = 0;
	while (environ[env_count])
		env_count++;
	cmd->environment_variables = (char **)malloc((env_count + 1) \
		* sizeof(char *));
	if (!cmd->environment_variables)
		return ;
	i = 0;
	while (i < env_count)
	{
		cmd->environment_variables[i] = strdup(environ[i]);
		if (!cmd->environment_variables[i])
		{
			while (i > 0)
				free(cmd->environment_variables[--i]);
			free(cmd->environment_variables);
			cmd->environment_variables = NULL;
			return ;
		}
		i++;
	}
	cmd->environment_variables[env_count] = NULL;
}

t_command	*initialize_command_state(t_input_state *input_state)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	ft_memset(cmd, 0, sizeof(t_command));
	copy_environment_variables(cmd);
	input_state->in_single_quotes = 0;
	input_state->in_double_quotes = 0;
	input_state->heredoc_active = 0;
	input_state->heredoc_delim = NULL;
	cmd->exit_status = 0;
	return (cmd);
}

void	execute_loop(t_input_state *input_state, t_command *cmd, \
		t_parse_context *context)
{
	char			*command;

	if (!cmd || !context)
		return ;
	while (1)
	{
		command = get_command();
		if (!command)
			break ;
		if (process_command_loop(&command, input_state, cmd, context))
			continue ;
		free_command_and_delim(&command, &input_state->heredoc_delim);
	}
	if (input_state->heredoc_delim)
		free(input_state->heredoc_delim);
	free_command(cmd);
}

t_parse_context	*initialize_parse_context(void)
{
	t_parse_context	*context;

	context = (t_parse_context *)malloc(sizeof(t_parse_context));
	if (!context)
	{
		write(STDERR_FILENO, "Memory allocation error\n", 24);
		exit(1);
	}
	context->head = NULL;
	context->current = NULL;
	context->max_args = 128;
	context->arg_index = 0;
	context->state = NULL;
	return (context);
}

int	main(void)
{
	int					exit_status;
	t_input_state		input_state;
	t_command			*cmd;
	t_parse_context		*context;

	cmd = initialize_command_state(&input_state);
	context = initialize_parse_context();
	handle_signals();
	exit_status = 0;
	setup_terminal(&g_orig_term);
	execute_loop(&input_state, cmd, context);
	restore_terminal_settings();
	free_command_at_end(cmd, context);
	return (exit_status);
}
