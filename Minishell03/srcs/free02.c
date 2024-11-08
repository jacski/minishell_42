/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free02.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:23:55 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:23:57 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_command(t_command *command)
{
	if (command == NULL)
		return ;
	free_command_fields(command);
	free_arguments(command);
	free_files(command);
	free_heredoc(command);
}

void	free_command_list(t_command *head)
{
	t_command	*current;
	t_command	*next;

	current = head;
	while (current != NULL)
	{
		next = current->next;
		free_command(current);
		free(current);
		current = next;
	}
}

void	free_command_at_end(t_command *command, t_parse_context *context)
{
	free_environment_variables(command);
	free(command);
	free(context);
}
