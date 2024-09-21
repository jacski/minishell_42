/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 17:42:35 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/06 17:42:38 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*expand_variables(char *token)
{
	char	*var_value;

	if (token[0] == '$')
	{
		var_value = getenv(token + 1);
		if (var_value)
			return (strdup(var_value));
		else
			return (strdup(""));
	}
	return (strdup(token));
}

void	expand_all_arguments(t_command *head)
{
	t_command	*current;
	char		*expanded_arg;
	int			i;

	current = head;
	while (current != NULL)
	{
		i = 0;
		while (current->arguments && current->arguments[i] != NULL)
		{
			expanded_arg = expand_variables(current->arguments[i]);
			free(current->arguments[i]);
			current->arguments[i] = expanded_arg;
			i++;
		}
		current = current->next;
	}
}

char	*remove_quotes(char *str)
{
	size_t	len;
	char	*new_str;

	len = strlen(str);
	if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"') || \
		(str[0] == '\'' && str[len - 1] == '\'')))
	{
		new_str = malloc(len - 1);
		if (!new_str)
		{
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		strncpy(new_str, str + 1, len - 2);
		new_str[len - 2] = '\0';
		return (new_str);
	}
	return (strdup(str));
}

t_command	*create_command(int max_args)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	memset(cmd, 0, sizeof(t_command));
	cmd->arguments = malloc(sizeof(char *) * max_args);
	if (!cmd->arguments)
	{
		perror("malloc");
		free(cmd);
		exit(EXIT_FAILURE);
	}
	return (cmd);
}

void	handle_pipeline(t_command **current, int *arg_index, int max_args)
{
	(*current)->arguments[*arg_index] = NULL;
	(*current)->next = create_command(max_args);
	*current = (*current)->next;
	*arg_index = 0;
}
