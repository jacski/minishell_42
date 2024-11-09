/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line01.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:11:01 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:11:03 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*get_env_value(char *var_name, t_command *cmd)
{
	int		i;
	size_t	var_len;

	var_len = ft_strlen(var_name);
	i = 0;
	while (cmd->environment_variables[i] != NULL)
	{
		if (my_strncmp(cmd->environment_variables[i], var_name, var_len) == 0 \
			&& cmd->environment_variables[i][var_len] == '=')
			return (ft_strdup(cmd->environment_variables[i] + var_len + 1));
		i++;
	}
	return (ft_strdup(""));
}

char	*expand_variables(char *token, t_command *cmd)
{
	char	*dollar_sign;
	char	*prefix;
	char	*var_value;
	char	*expanded_token;

	if (token[0] == '\'' && token[ft_strlen(token) - 1] == '\'')
		return (ft_strdup(token));
	dollar_sign = strchr(token, '$');
	if (!dollar_sign)
		return (ft_strdup(token));
	prefix = ft_strndup(token, dollar_sign - token);
	var_value = get_env_value(dollar_sign + 1, cmd);
	expanded_token = ft_strjoin(prefix, var_value);
	free(prefix);
	free(var_value);
	return (expanded_token);
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
			if (current->expand_flags[i])
			{
				expanded_arg = expand_variables(current->arguments[i] \
					, current);
				free(current->arguments[i]);
				current->arguments[i] = expanded_arg;
			}
			i++;
		}
		current = current->next;
	}
}

char	*remove_quotes(char *str)
{
	size_t	len;
	char	*new_str;

	len = ft_strlen(str);
	if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"') || \
		(str[0] == '\'' && str[len - 1] == '\'')))
	{
		new_str = malloc(len - 1);
		if (!new_str)
		{
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		my_strncpy(new_str, str + 1, len - 2);
		new_str[len - 2] = '\0';
		return (new_str);
	}
	return (ft_strdup(str));
}

void	init_command(t_command *cmd)
{
	cmd->command = NULL;
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
	cmd->has_heredoc = false;
	cmd->next = NULL;
}
