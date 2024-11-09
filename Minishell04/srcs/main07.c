/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main07.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 12:31:34 by jkalinow          #+#    #+#             */
/*   Updated: 2024/11/09 12:31:38 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

struct termios		g_orig_term;

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
