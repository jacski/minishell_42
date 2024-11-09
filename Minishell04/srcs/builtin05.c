/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin05.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:34:59 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:35:02 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	restore_std(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}

int	handle_redirection_builtin(t_command *cmd, int *saved_stdin, \
		int *saved_stdout)
{
	if (handle_input_redirection_builtin(cmd, saved_stdin) < 0)
		return (1);
	if (handle_output_redirection(cmd, saved_stdout) < 0)
	{
		restore_std(*saved_stdin, -1);
		return (1);
	}
	return (0);
}

int	execute_builtin(t_command *cmd, t_parse_context *context)
{
	int	saved_stdin;
	int	saved_stdout;
	int	result;

	saved_stdin = -1;
	saved_stdout = -1;
	if (handle_redirection_builtin(cmd, &saved_stdin, &saved_stdout) < 0)
		return (1);
	result = execute_builtin_command(cmd, context);
	restore_std(saved_stdin, saved_stdout);
	return (result);
}
