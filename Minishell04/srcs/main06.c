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

int	process_command_loop(char **command, t_input_state *input_state, \
		t_command *cmd, t_parse_context *context)
{
	if (handle_empty_command(command))
		return (1);
	if (process_input_and_expand(command, input_state, cmd))
		return (1);
	if (*command && parse_and_execute_command(command, cmd, context))
		return (1);
	if (*command)
	{
		free(*command);
		*command = (NULL);
	}
	return (0);
}

char	*get_command(t_command *cmd, t_parse_context *context)
{
	char	*command;

	command = readline("minishell> ");
	if (command == NULL)
	{
		write(STDOUT_FILENO, "\nExiting...\n", 12);
		free_command_at_end(cmd, context);
		exit(EXIT_SUCCESS);
	}
	if (*command == '\0')
	{
		free(command);
		return (NULL);
	}
	return (command);
}

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
		command = get_command(cmd, context);
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
