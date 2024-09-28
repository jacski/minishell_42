// to do absolute path
// to do memory leaks

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <errno.h>
# include <stdbool.h>
# include <sys/wait.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>

extern char				**environ;
struct termios	g_orig_term;


typedef struct TokenizerState
{
	char	*saveptr;
	bool	in_quotes;
	char	quote_char;
	char	heredoc_delim[256];
	bool	prev_heredoc;
}	t_TokenizerState;

typedef struct Command

{
	char			*command;
	char			**arguments;
	bool			prev_input_file;
	char			*input_file;
	bool			prev_output_file;
	char			*output_file;
	bool			prev_append_file;
	char			*append_file;
	char			*heredoc_delim;
	char			*close_heredoc_delim;
	int				input_fd;
	int				output_fd;
	int				pipe_fd[2];
	int				executed_successfully;
	char			**environment_variables;
	struct Command	*next;
	bool			prev_heredoc;
	bool		next_heredoc;
	char            *heredoc_content;
}	t_command;

typedef struct s_parse_context {
	t_command	*head;
	t_command	*current;
	t_TokenizerState *state;
	int			max_args;
	int			arg_index;
} t_parse_context;

/*/ ******************   check funtion   ******************************
void	print_command_structure(t_command *cmd)
{
	int i;

	while (cmd != NULL)
	{
		if (cmd->command)
			printf("Command: %s\n", cmd->command);
		if (cmd->arguments)
		{
			printf("Arguments:\n");
			for (i = 0; cmd->arguments[i] != NULL; i++)
			{
				printf("\t[%d]: *%s*\n", i, cmd->arguments[i]);
			}
		}

		// Print file redirections
		if (cmd->input_file)
			printf("Input file: %s\n", cmd->input_file);
		if (cmd->output_file)
			printf("Output file: %s\n", cmd->output_file);
		if (cmd->append_file)
			printf("Append file: %s\n", cmd->append_file);

		// Print heredoc information
		if (cmd->heredoc_delim)
			printf("heredoc_delim: %s\n", cmd->heredoc_delim);
		if (cmd->heredoc_content)
			printf("heredoc_content: %s\n", cmd->heredoc_content);
		if (cmd->close_heredoc_delim)
			printf("close_heredoc_delim: %s\n", cmd->close_heredoc_delim);

		// Print file descriptors (if you're using them)
		printf("Input FD: %d\n", cmd->input_fd);
		printf("Output FD: %d\n", cmd->output_fd);

		// Print pipe info (if any)
		if (cmd->pipe_fd[0] != -1 && cmd->pipe_fd[1] != -1)
			printf("Pipe FD: [%d, %d]\n", cmd->pipe_fd[0], cmd->pipe_fd[1]);

		// Check if the command executed successfully
		printf("Executed successfully: %d\n", cmd->executed_successfully);

		// Move to the next command in the pipeline (if any)
		cmd = cmd->next;
	}
}

//*/

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	ft_isalnum(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9'))
		return (1);
	return (0);
}

char	*ft_strchr(const char *str, int character)
{
	if (character < 0 || character > 255)
		return (NULL);
	while (*str != '\0')
	{
		if (*str == character)
			return ((char *)str);
		str++;
	}
	if (character == '\0')
		return ((char *)str);
	return (NULL);
}

void	*ft_memset(void *ptr, int value, size_t len)
{
	unsigned char	*p;
	unsigned char	v;
	size_t			i;

	i = 0;
	p = ptr;
	v = (unsigned char)value;
	while (i < len)
	{
		p[i] = v;
		i++;
	}
	return (ptr);
}

char	*ft_strcpy(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s2[i])
	{
		s1[i] = s2[i];
		i++;
	}
	s1[i] = '\0';
	return (s1);
}

char	*ft_strdup(const char *str)
{
	size_t	length;
	size_t	i;
	char	*copy;

	if (str == NULL)
		return (NULL);
	length = 0;
	while (str[length])
		length++;
	copy = (char *)malloc(length + 1);
	if (copy == NULL)
		return (NULL);
	i = 0;
	while (str[i])
	{
		copy[i] = str[i];
		i++;
	}
	copy[i] = '\0';
	return (copy);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while ((s1[i] || s2[i]) && s1[i] == s2[i])
		i++;
	return (s1[i] - s2[i]);
}

size_t	my_strspn(const char *s1, const char *s2)
{
	size_t		count;
	const char	*p1;
	const char	*p2;

	count = 0;
	p1 = s1;
	while (*p1)
	{
		p2 = s2;
		while (*p2)
		{
			if (*p1 == *p2)
				break ;
			p2++;
		}
		if (*p2 == '\0')
			break ;
		count++;
		p1++;
	}
	return (count);
}

char	*initialize_token(t_TokenizerState *state, char *str, \
		const char *delim)
{
	if (str != NULL)
		state->saveptr = str;
	else if (state->saveptr == NULL)
		return (NULL);
	state->saveptr += my_strspn(state->saveptr, delim);
	if (*state->saveptr == '\0')
	{
		state->saveptr = NULL;
		return (NULL);
	}
	return (state->saveptr);
}

bool	entering_quoted_string(t_TokenizerState *state)
{
	if ((*state->saveptr == '"' || *state->saveptr == '\'') \
		&& !state->in_quotes)
	{
		state->in_quotes = true;
		state->quote_char = *state->saveptr;
		state->saveptr++;
		return (true);
	}
	return (false);
}

bool	exiting_quoted_string(t_TokenizerState *state)
{
	if (*state->saveptr == state->quote_char && state->in_quotes)
	{
		state->in_quotes = false;
		state->quote_char = '\0';
		state->saveptr++;
		return (true);
	}
	return (false);
}

bool	handle_delimiters(t_TokenizerState *state, const char *delim)
{
	if (!state->in_quotes && strchr(delim, *state->saveptr))
	{
		*state->saveptr = '\0';
		state->saveptr++;
		return (true);
	}
	return (false);
}

char	*handle_pipe(t_TokenizerState *state)
{
	*state->saveptr = '\0';
	state->saveptr++;
	return ("|");
}

char	*handle_non_token_redirection(t_TokenizerState *state, char *token)
{
	int		len;
	char	*separ_spec_char;
	char	*result;

	len = state->saveptr - token;
	separ_spec_char = (char *)malloc((len + 1) * sizeof(char));
	if (separ_spec_char == NULL)
	{
		perror("Failed to allocate memory");
		return (NULL);
	}
	strncpy(separ_spec_char, token, len);
	separ_spec_char[len] = '\0';
	
	result = ft_strdup(separ_spec_char);
	printf("332 result %s\n", result);
	free(separ_spec_char);
	return (result);
}



char	*handle_double_redirection(t_TokenizerState *state)
{
	char	*saveptr_backup;
	int	i;

	if(*state->saveptr == '<' && *(state->saveptr + 1) == '<')
	{		
		saveptr_backup = state->saveptr + 2;
		state->saveptr += 2;
		while (*state->saveptr == ' ' || *state->saveptr == '\t')
			state->saveptr++;
		i = 0;
		while(!strchr(" \t\n\0", state->saveptr[i]))
			i++;

		strncpy(state->heredoc_delim, state->saveptr, i);
		state->heredoc_delim[i] = '\0'; 

		state->saveptr = saveptr_backup;
		return ("<<");
	}
	state->saveptr += 2;
	return (">>");
}

char	*handle_single_redirection(t_TokenizerState *state)
{
	if (*state->saveptr == '<')
	{
		state->saveptr++;
		return ("<");
	}
	state->saveptr++;
	return (">");
}

char	*handle_specific_redirection(t_TokenizerState *state)
{

	if (((*state->saveptr == '>' && *(state->saveptr + 1) == '>') || \
		(*state->saveptr == '<' && *(state->saveptr + 1) == '<')))
		return (handle_double_redirection(state));
	else
		return (handle_single_redirection(state));
}

char	*handle_redirection_strtok(t_TokenizerState *state, char *token)
{
	char	*result;

	if (state->saveptr != token)
	{
		result = handle_non_token_redirection(state, token);
		if (result)
			return (result);
	}
	else
		return (handle_specific_redirection(state));
	return (NULL);
}

bool	handle_special_characters(t_TokenizerState *state, char **token)
{

	if (*state->saveptr == '|')
	{
		*token = handle_pipe(state);
		return (true);
	}
	if (strchr("<>", *state->saveptr) != NULL)
	{


		*token = handle_redirection_strtok(state, *token);

		return (true);
	}
	return (false);
}

bool is_heredoc_delimiter(t_TokenizerState *state)
{
	size_t heredoc_delim_len = strlen(state->heredoc_delim);
	char *ptr = state->saveptr;

	if (strncmp(ptr, state->heredoc_delim, heredoc_delim_len) == 0)
	{
		if (*(ptr + heredoc_delim_len) == '\n' || *(ptr + heredoc_delim_len) == '\0')
		{
			ptr--;
			while (ptr > state->saveptr && strchr(" \t", *ptr))
				ptr--;
			if (ptr == state->saveptr || *ptr == '\n')
				return true;
		}
	}
	return false;
}

char *process_heredoc_content(t_TokenizerState *state)
{
	char *token = state->saveptr;
	while (*state->saveptr)
	{
		if (is_heredoc_delimiter(state))
		{
			if (*(state->saveptr - 1) == '\n')
				*(state->saveptr - 1) = '\0';
			state->prev_heredoc = false;
			break;
		}
		state->saveptr++;
	}
	return token;
}

bool process_quoted_string(t_TokenizerState *state)
{
	if (entering_quoted_string(state))
		return true;
	if (exiting_quoted_string(state))
		return true;
	return false;
}

size_t get_var_name_len(const char *str)
{
    size_t len = 0;
    str++;  // Skip the '$'
    
    while (*str && (isalnum(*str) || *str == '_'))
    {
        len++;
        str++;
    }

    return len;
}

void replace_with_env_value(t_TokenizerState *state, const char *var_value)
{
    char *new_token;
    size_t token_len = strlen(state->saveptr);
    size_t var_value_len = strlen(var_value);

    // Calculate new token length: original token length - length of the variable + length of the variable value
    size_t new_len = token_len - get_var_name_len(state->saveptr) + var_value_len;

    // Allocate new memory for the expanded token
    new_token = malloc(new_len + 1);  // +1 for null terminator
    if (!new_token)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Copy part before the variable
    char *dollar_pos = strchr(state->saveptr, '$');  // Find the position of the dollar sign
    size_t prefix_len = dollar_pos - state->saveptr;
    strncpy(new_token, state->saveptr, prefix_len);

    // Append the environment variable value
    strcpy(new_token + prefix_len, var_value);

    // Append the remaining part of the original string after the variable name
    const char *suffix = state->saveptr + prefix_len + get_var_name_len(state->saveptr);
    strcpy(new_token + prefix_len + var_value_len, suffix);

    // Update saveptr to point to the new token
    state->saveptr = new_token;
}

void process_env_variable(t_TokenizerState *state)
{
	char *start = state->saveptr + 1;  // Skip the '$' character
	char var_name[256];  // Adjust size as necessary
	int i = 0;

	// Extract the variable name
	while (*start && (ft_isalnum(*start) || *start == '_'))
	{
		var_name[i++] = *start++;
	}
	var_name[i] = '\0';

	// Find the environment variable value
	char *var_value = getenv(var_name);
	if (var_value)
	{
		// Substitute the variable in place of $VAR_NAME
		replace_with_env_value(state, var_value);  // Implement this to handle replacement
	}

	state->saveptr = start;  // Move the pointer past the variable name
}

char *handle_quoted_strings(t_TokenizerState *state, const char *delim)
{
	char *token = state->saveptr;

	while (*state->saveptr)
	{
		if (process_quoted_string(state))
			continue;
		if (state->prev_heredoc)
			return process_heredoc_content(state);
		if (strchr("<>|", *state->saveptr) && !state->in_quotes)
		{
			if (handle_special_characters(state, &token))
				return token;
		}
		if (state->in_quotes && *state->saveptr == '$')
		{
			process_env_variable(state);  // Handle the variable expansion
			continue;
		}
		if (handle_delimiters(state, delim) && !state->prev_heredoc)
			break;
		state->saveptr++;
	}
	if (state->saveptr == NULL)
		return NULL;
	if (ft_strcmp(state->heredoc_delim, token) == 0)
		 state->prev_heredoc = true;
	return token;
}

char	*my_strtok(t_TokenizerState *state, char *str, const char *delim)
{
	char					*token;

	if (str != NULL)
	{
		state->saveptr = str;
		state->in_quotes = false;
		state->quote_char = '\0';
		state->heredoc_delim[0] = '\0';
		state->prev_heredoc = false;
	}
	token = initialize_token(state, str, delim);
	if (!token)
		return (NULL);

	token = handle_quoted_strings(state, delim);
	if (!token)
		return (NULL);

	if (*state->saveptr == '\0')
		state->saveptr = NULL;
	return (token);
}

// ******************* parse_line ***************************

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
			if (current->arguments[i] != NULL) {
			    free(current->arguments[i]);  // Free previous memory
			}
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
	cmd->command = NULL;
	cmd->prev_input_file  = false;
	cmd->input_file = NULL;
	cmd->prev_output_file  = false;
	cmd->output_file = NULL;
	cmd->prev_append_file  = false;
	cmd->append_file = NULL;
	cmd->heredoc_content = NULL;
	cmd->heredoc_delim = NULL;
	cmd->close_heredoc_delim = NULL;
	cmd->input_fd = -1;
	cmd->output_fd = -1;
	cmd->pipe_fd[0] = -1;
	cmd->pipe_fd[1] = -1;
	cmd->executed_successfully = 0;
	cmd->prev_heredoc = false;
	cmd->next_heredoc = false;
	cmd->environment_variables = NULL;
	cmd->next = NULL;
	cmd->arguments = malloc(sizeof(char *) * (max_args + 1));
	if (!cmd->arguments)
	{
		perror("malloc");
		free(cmd);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i <= max_args; i++) {
		cmd->arguments[i] = NULL;
	}
//*/	
	cmd->environment_variables = malloc(sizeof(char *) * (max_args + 1));
	if (!cmd->environment_variables)
	{
		perror("malloc");
		free(cmd);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i <= 64; i++) {
		cmd->environment_variables[i] = NULL;
	}
//*/
	return (cmd);
}

void	handle_pipeline(t_command **current, int *arg_index, int max_args)
{
	(*current)->arguments[*arg_index] = NULL;
	(*current)->next = create_command(max_args);
	*current = (*current)->next;
	*arg_index = 0;
}

void	handle_input_redirection(t_command *current, char **token, t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_input_file = true;
//		current->input_file = strdup(*token);
}

//*/
void	handle_output_redirection_parsel(t_command *current, char **token, t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_output_file = true;
//		current->output_file = strdup(*token);
}
//*/

void	handle_append_redirection(t_command *current, char **token, t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_append_file = true;
//		current->append_file = strdup(*token);
}

void	handle_heredoc_redirection(t_command *current, char **token, t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_heredoc = true;
}


void	handle_redirection_parse_l(t_command *current, char **token)
{
	t_TokenizerState state = {0};
	state.saveptr = *token;

	if (strcmp(*token, "<") == 0)
		handle_input_redirection(current, token, &state);
	else if (strcmp(*token, ">") == 0)
		handle_output_redirection_parsel(current, token, &state);
	else if (strcmp(*token, ">>") == 0)
		handle_append_redirection(current, token, &state);
	else if (strcmp(*token, "<<") == 0)
		handle_heredoc_redirection(current, token, &state);
}

void	handle_too_many_arguments(t_command *current)
{
	write(2, "Too many arguments for command: \n", 33);
	write(2, current->command, strlen(current->command));
	write(2, "\n", 1);
}

void	handle_heredoc(t_command *current, char *token)
{
	if (current->heredoc_delim && current->heredoc_content == NULL)
	{
		if (token != NULL)
		{
			current->heredoc_content = strdup(token);
			current->prev_heredoc = false;
		}
		current->next_heredoc = true;
	}
	if (current->heredoc_delim == NULL)
		current->heredoc_delim = strdup(token);
}

void	handle_input_file(t_command *current, char *token)
{
	if (current->prev_input_file)
	{
		if (token != NULL)
		{
			current->input_file = strdup(token);
			current->prev_input_file = false;
		}
	}
}

void	handle_output_file(t_command *current, char *token)
{
	if (current->prev_output_file)
	{
		if (token != NULL)
		{
			current->output_file = strdup(token);
			current->prev_output_file = false;
		}
	}
}

void	handle_append_file(t_command *current, char *token)
{
	if (current->prev_append_file)
	{
		if (token != NULL)
		{
			current->append_file = strdup(token);
			current->prev_append_file = false;
		}
	}
}

void	handle_next_heredoc(t_command *current, char *token)
{
	if (token != NULL)
	{
		if (current->close_heredoc_delim != NULL) {
		    free(current->close_heredoc_delim);
		}
		current->close_heredoc_delim = strdup(token);
		current->next_heredoc = false;
	}
}


void	handle_arguments(t_command *current, char *token, int *arg_index, int max_args)
{
	char	*unquoted_token;

	unquoted_token = remove_quotes(token);
	if (*arg_index == 0)
	{
		if (current->command != NULL) {
		    free(current->command);  // Free previously allocated memory
		}
		current->command = strdup(unquoted_token);;
	}
	if (*arg_index >= max_args - 1)
	{
		handle_too_many_arguments(current);
		free(unquoted_token);
		return;
	}
	
	if (current->prev_heredoc)
		handle_heredoc(current, token);
	else if (!current->next_heredoc && !current->prev_input_file && \
		!current->prev_output_file&& !current->prev_append_file)
	{
		if (current->arguments[*arg_index] != NULL)
		    free(current->arguments[*arg_index]);

		current->arguments[(*arg_index)++] = strdup(unquoted_token);
	}
	else if (current->prev_input_file)
		handle_input_file(current, token);
	else if (current->prev_output_file)
		handle_output_file(current, token);
	else if (current->prev_append_file)
		handle_append_file(current, token);
	else
		handle_next_heredoc(current, token);
	free(unquoted_token);
}

void	process_token(t_parse_context *context, char *token)
{
	if (!context->head)
	{
		context->head = create_command(context->max_args);
		context->current = context->head;
	}
	if (strcmp(token, "|") == 0)
		handle_pipeline(&context->current, &context->arg_index, context->max_args);
	else if (strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || 
		strcmp(token, ">>") == 0 || strcmp(token, "<<") == 0)
		handle_redirection_parse_l(context->current, &token);
	else
		handle_arguments(context->current, token, &context->arg_index, context->max_args);
}

void	process_tokens(t_parse_context *context, char *line)
{
	char	*token;
	context->state->saveptr = line;
	token = my_strtok(context->state, line, " \t\n");

	while (token != NULL)
	{
		process_token(context, token);
		token = my_strtok(context->state, NULL, " \t\n");
	}

	if (context->current)
		context->current->arguments[context->arg_index] = NULL;
	
	expand_all_arguments(context->head);
	
}


t_command	*parse_line(char *line)
{
	t_parse_context	context;
	t_TokenizerState state = {0};

//	state = {0};

/*/
	state.saveptr = NULL;
	state.in_quotes = false;
	state.quote_char = '\0';
	state.heredoc_delim[0] = '\0';
	state.prev_heredoc = false;
//*/	
	context.head = NULL;
	context.current = NULL;
	context.max_args = 64;	
	context.arg_index = 0;
	context.state = &state;
	
	process_tokens(&context, line);
	
	if (state.in_quotes)
    {
        write(2, "Error: Unclosed quote at the end of input.\n", 43);  // Print error to stderr
        // You can handle cleanup or return an error-specific value if needed
    }

	return (context.head);
}

// ******************            is_builtin           **************************

int	handle_input_redirection_builtin(t_command *cmd, int *saved_stdin)
{
	int	input_fd;

	if (cmd->input_file)
	{
		input_fd = open(cmd->input_file, O_RDONLY);
		if (input_fd < 0)
		{
			perror("open input file");
			return (-1);
		}
		*saved_stdin = dup(STDIN_FILENO);
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	return (0);
}

int	open_output_file(const char *file, int flags, const char *error_message)
{
	int	output_fd;

	output_fd = open(file, flags, 0644);
	if (output_fd < 0)
	{
		perror(error_message);
		return (-1);
	}
	return (output_fd);
}

void	redirect_output(int output_fd, int *saved_stdout)
{
	*saved_stdout = dup(STDOUT_FILENO);
	dup2(output_fd, STDOUT_FILENO);
	close(output_fd);
}

int	handle_output_redirection(t_command *cmd, int *saved_stdout)
{
	int	output_fd;

	if (cmd->output_file)
	{
		output_fd = open_output_file(cmd->output_file, O_WRONLY | O_CREAT | \
			O_TRUNC, "open output file");
		if (output_fd < 0)
			return (-1);
		redirect_output(output_fd, saved_stdout);
	}
	else if (cmd->append_file)
	{
		output_fd = open_output_file(cmd->append_file, O_WRONLY | O_CREAT | \
			O_APPEND, "open append file");
		if (output_fd < 0)
			return (-1);
		redirect_output(output_fd, saved_stdout);
	}
	return (0);
}

int	execute_cd(t_command *cmd)
{
	if (cmd->arguments[1] == NULL)
		chdir(getenv("HOME"));
	else
	{
		if (chdir(cmd->arguments[1]) != 0)
		{
			perror("cd");
			return (1);
		}
	}
	return (0);
}

int	execute_echo(t_command *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->arguments[1] && strcmp(cmd->arguments[1], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (cmd->arguments[i])
	{
		write(1, cmd->arguments[i], strlen(cmd->arguments[i]));
		if (cmd->arguments[i + 1])
		{
			write(1, " ", 1);
		}
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}

int	handle_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
	{
		perror("getcwd");
		return (1);
	}
	return (0);
}

int	handle_export(t_command *cmd)
{
	int		i;
	char	*env;

	i = 1;
	while (cmd->arguments[i] != NULL)
	{
		env = strchr(cmd->arguments[i], '=');
		if (env)
		{
			*env = '\0';
			setenv(cmd->arguments[i], env + 1, 1);
		}
		else
			write(STDERR_FILENO, "Invalid format for export\n", 26);
		i++;
	}
	return (0);
}

int	handle_unset(t_command *cmd)
{
	int	i;

	i = 1;
	while (cmd->arguments[i] != NULL)
	{
		unsetenv(cmd->arguments[i]);
		i++;
	}
	return (0);
}

int	handle_env(void)
{
	int	i;

	i = 0;
	while (__environ[i] != NULL)
	{
		puts(__environ[i]);
		i++;
	}
	return (0);
}

int	handle_exit(void)
{
	write(1, "Exiting...\n", 12);
	exit(0);
}

int	is_builtin(t_command *cmd)
{
	char	*builtins[8];
	int		i;

	builtins[0] = "cd";
	builtins[1] = "exit";
	builtins[2] = "echo";
	builtins[3] = "pwd";
	builtins[4] = "export";
	builtins[5] = "unset";
	builtins[6] = "env";
	builtins[7] = NULL;
	i = 0;
	while (builtins[i] != NULL)
	{
		if (strcmp(cmd->command, builtins[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

int	execute_builtin_command(t_command *cmd)
{
	if (strcmp(cmd->command, "cd") == 0)
		return (execute_cd(cmd));
	else if (strcmp(cmd->command, "exit") == 0)
		return (handle_exit());
	else if (strcmp(cmd->command, "echo") == 0)
		return (execute_echo(cmd));
	else if (strcmp(cmd->command, "pwd") == 0)
		return (handle_pwd());
	else if (strcmp(cmd->command, "export") == 0)
		return (handle_export(cmd));
	else if (strcmp(cmd->command, "unset") == 0)
		return (handle_unset(cmd));
	else if (strcmp(cmd->command, "env") == 0)
		return (handle_env());
	else
	{
		write(STDERR_FILENO, "Unknown built-in command: /n", 27);
		write(STDERR_FILENO, cmd->command, strlen(cmd->command));
		write(STDERR_FILENO, "\n", 1);
		return (1);
	}
}

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

int	execute_builtin(t_command *cmd)
{
	int	saved_stdin;
	int	saved_stdout;
	int	result;

	saved_stdin = -1;
	saved_stdout = -1;
	if (handle_redirection_builtin(cmd, &saved_stdin, &saved_stdout) < 0)
		return (1);
	result = execute_builtin_command(cmd);
	restore_std(saved_stdin, saved_stdout);
	return (result);
}

// ******************                pipe             **************************

int	create_pipe(int pipe_fd[2])
{
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

void	exec_command(t_command *cmd)
{
	if (execvp(cmd->arguments[0], cmd->arguments) == -1)
	{
		perror("execvp");
		exit(EXIT_FAILURE);
	}
}

int	setup_input_redirection(t_command *cmd, int *in_fd)
{
	if (cmd->input_file != NULL)
	{
		*in_fd = open(cmd->input_file, O_RDONLY);
		if (*in_fd == -1)
		{
			perror("open input file");
			return (-1);
		}
	}
	if (*in_fd != 0)
	{
		if (dup2(*in_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 input");
			return (-1);
		}
		close(*in_fd);
	}
	return (0);
}

int	setup_pipe_output(int pipe_fd[2])
{
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		perror("dup2 output");
		return (-1);
	}
	close(pipe_fd[1]);
	close(pipe_fd[0]);
	return (0);
}

int	setup_file_output(t_command *cmd)
{
	int	out_fd;

	if (cmd->output_file != NULL)
	{
		out_fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1)
		{
			perror("open output file");
			return (-1);
		}
		if (dup2(out_fd, STDOUT_FILENO) == -1)
		{
			perror("dup2 output file");
			return (-1);
		}
		close(out_fd);
	}
	return (0);
}

int	setup_pipes(t_command *cmd)
{
	t_command	*current_cmd;

	if (cmd == NULL)
	{
		write(STDERR_FILENO, "Command is NULL\n", 16);
		return (-1);
	}
	current_cmd = cmd;
	while (current_cmd->next != NULL)
	{
		if (pipe(current_cmd->pipe_fd) == -1)
		{
			write(STDERR_FILENO, "Error creating pipe\n", 20);
			return (-1);
		}
		current_cmd->next->input_fd = current_cmd->pipe_fd[0];
		current_cmd = current_cmd->next;
	}
	return (0);
}

int	setup_redirections(t_command *cmd, int in_fd, int pipe_fd[2])
{
	if (setup_input_redirection(cmd, &in_fd) == -1)
		return (-1);
	if (cmd->next != NULL)
	{
		if (setup_pipe_output(pipe_fd) == -1)
			return (-1);
	}
	else
	{
		if (setup_file_output(cmd) == -1)
			return (-1);
	}
	return (0);
}

int	handle_fork(t_command *cmd, int in_fd, int pipe_fd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
	{
		if (setup_redirections(cmd, in_fd, pipe_fd) == -1)
			exit(EXIT_FAILURE);
		exec_command(cmd);
	}
	return (0);
}

int	process_command(t_command *cmd, int *in_fd)
{
	int	pipe_fd[2];

	if (cmd->next != NULL)
	{
		if (create_pipe(pipe_fd) == -1)
			return (-1);
	}
	if (handle_fork(cmd, *in_fd, pipe_fd) == -1)
		return (-1);
	if (cmd->next != NULL)
		close(pipe_fd[1]);
	if (*in_fd != 0)
		close(*in_fd);
	if (cmd->next != NULL)
		*in_fd = pipe_fd[0];
	else
		*in_fd = 0;
	return (0);
}

int	execute_pipeline(t_command *cmd)
{
	t_command	*current_cmd;
	int			in_fd;
	int			status;

	current_cmd = cmd;
	in_fd = 0;
	while (current_cmd != NULL)
	{
		if (process_command(current_cmd, &in_fd) == -1)
			return (-1);
		current_cmd = current_cmd->next;
	}
	while (wait(&status) > 0)
		;
	return (0);
}

// ******************        initialize_command       **************************

int	check_command_null(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Command is NULL\n";
	if (cmd == NULL)
	{
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		return (-1);
	}
	return (0);
}

int	handle_input_redirection_initialize_c(t_command *cmd)
{
	const char	*error_msg = "Error opening input file: ";

	error_msg = "Error opening input file: ";
	if (cmd->input_file != NULL)
	{
		cmd->input_fd = open(cmd->input_file, O_RDONLY);
		if (cmd->input_fd == -1)
		{
			write(STDERR_FILENO, error_msg, strlen(error_msg));
			write(STDERR_FILENO, cmd->input_file, strlen(cmd->input_file));
			write(STDERR_FILENO, "\n", 1);
			return (-1);
		}
	}
	return (0);
}

int	create_output_file(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Error creating output file: ";
	cmd->output_fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (cmd->output_fd == -1)
	{
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		write(STDERR_FILENO, cmd->output_file, strlen(cmd->output_file));
		write(STDERR_FILENO, "\n", 1);
		return (-1);
	}
	return (0);
}

int	append_output_file(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Error appending to output file: ";
	cmd->output_fd = open(cmd->append_file, \
		O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (cmd->output_fd == -1)
	{
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		write(STDERR_FILENO, cmd->append_file, strlen(cmd->append_file));
		write(STDERR_FILENO, "\n", 1);
		return (-1);
	}
	return (0);
}

void	set_environment_variables(t_command *cmd)
{
	int	i;

	if (cmd->environment_variables != NULL)
	{
		i = 0;
		while (cmd->environment_variables[i])
		{
			putenv(cmd->environment_variables[i]);
			i++;
		}
	}
}

int	initialize_command(t_command *cmd)
{
	if (check_command_null(cmd) == -1)
		return (-1);
	if (handle_input_redirection_initialize_c(cmd) == -1)
		return (-1);
	if (cmd->output_file != NULL)
	{
		if (create_output_file(cmd) == -1)
			return (-1);
	}
	else if (cmd->append_file != NULL)
	{
		if (append_output_file(cmd) == -1)
			return (-1);
	}
	set_environment_variables(cmd);
	cmd->executed_successfully = 0;
	return (0);
}

// ******************         execute_command         **************************

int	check_command(t_command *cmd)
{
	if (cmd == NULL)
	{
		write(STDERR_FILENO, "Command is NULL\n", 16);
		return (-1);
	}
	return (0);
}

pid_t	create_child_process(void)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		write(STDERR_FILENO, "Error creating child process\n", 29);
	}
	return (pid);
}

void	handle_redirection_process(t_command *cmd)
{
	int heredoc_pipe[2];
	if (cmd->heredoc_delim != NULL && ft_strcmp(cmd->heredoc_delim, \
		cmd->close_heredoc_delim) == 0)
	{	
		if (pipe(heredoc_pipe) == -1)
		{
			write(STDERR_FILENO, "Error creating heredoc pipe\n", 28);
			exit(EXIT_FAILURE);
		}
		write(heredoc_pipe[1], cmd->heredoc_content, strlen(cmd->heredoc_content));
		write(heredoc_pipe[1], "\n", 1);
		close(heredoc_pipe[1]);
		dup2(heredoc_pipe[0], STDIN_FILENO);
		close(heredoc_pipe[0]);
	}
/*/
	else if (ft_strcmp(cmd->heredoc_delim, \
		cmd->close_heredoc_delim) != 0)
		write(heredoc_pipe[1], "heredoc_delimiter\n", 18);
//*/	
	if (cmd->input_fd != -1)
	{
		dup2(cmd->input_fd, STDIN_FILENO);
		close(cmd->input_fd);
	}
	if (cmd->output_fd != -1)
	{
		dup2(cmd->output_fd, STDOUT_FILENO);
		close(cmd->output_fd);
	}
}



void	execute_command_process(t_command *cmd)
{
	if (execvp(cmd->command, cmd->arguments) == -1)
	{
		write(STDERR_FILENO, "Error executing command: ", 25);
		write(STDERR_FILENO, cmd->command, strlen(cmd->command));
		write(STDERR_FILENO, "\n", 1);
		exit(EXIT_FAILURE);
	}
}

int	wait_for_child_process(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		write(STDERR_FILENO, "Error waiting for child process\n", 32);
		return (-1);
	}
	return (status);
}

char *find_command_in_path(const char *cmd) {
    char *path = getenv("PATH");
    if (!path || strlen(path) == 0) {
        // PATH is not set or empty
        return NULL;
    }

    char *paths = strdup(path);  // Duplicate the PATH for tokenization
    char *dir = strtok(paths, ":");  // Split the PATH by ':'
    char *full_path = NULL;
    size_t cmd_len = strlen(cmd);

    while (dir != NULL) {
        size_t dir_len = strlen(dir);
        full_path = malloc(dir_len + cmd_len + 2);  // Allocate memory for "dir/cmd"

        if (!full_path) {
            perror("malloc");
            free(paths);
            return NULL;
        }

        // Construct the full path manually
        strcpy(full_path, dir);      // Copy directory part
        strcat(full_path, "/");      // Add the '/'
        strcat(full_path, cmd);      // Add the command part

        // Check if the command exists and is executable
        if (access(full_path, X_OK) == 0) {
            free(paths);
            return full_path;  // Return if found
        }

        free(full_path);  // Free the memory if not found
        dir = strtok(NULL, ":");
    }
    
    free(paths);
    return NULL;  // Command not found in PATH
}

int create_process(t_command *cmd) {
    pid_t pid;
    int status;

    // Check if PATH is set and find the command
    char *command_path = find_command_in_path(cmd->command);
    if (!command_path) {
        write(STDERR_FILENO, "Command not found or PATH is not set\n", 36);
        return (-1);
    }

    pid = fork();  // Fork a new process
    if (pid == -1) {
        free(command_path);  // Free the path memory on failure
        return (-1);  // Error in forking
    }

    if (pid == 0) {
        // Child process
        handle_redirection_process(cmd);

        // Use execve instead of execv
        extern char **environ;  // Get the environment variables
        if (execve(command_path, cmd->arguments, environ) == -1) {
            perror("execve");  // If execve fails
            exit(EXIT_FAILURE);
        }
    }

    // Parent process waits for the child process to finish
    status = waitpid(pid, NULL, 0);
    free(command_path);  // Free the allocated memory for the command path
    if (status == -1) {
        return (-1);  // Error while waiting
    }

	if (status == 0) {
	    cmd->executed_successfully = 1;
	} else {
	    cmd->executed_successfully = 0;
	}
	    return (0);
}

int	execute_command(t_command *cmd)
{
	if (is_builtin(cmd))
		return (execute_builtin(cmd));
	else if (cmd->next)
	{
		if (setup_pipes(cmd) == -1)
		{
			write(STDERR_FILENO, "Error setting up pipes for pipeline\n", 36);
			return (-1);
		}
		return (execute_pipeline(cmd));
	}
	else
	{
		if (initialize_command(cmd) == -1)
		{
			write(STDERR_FILENO, "Error initializing command\n", 27);
			return (-1);
		}
		return (create_process(cmd));
	}
}

// ******************            signals            ****************************

void sigquit_handler(int sig) {
    (void)sig;

        rl_on_new_line(); // Move to a new line
        rl_redisplay();
}


void handle_sigint(int sig) {
    (void)sig;

        write(1, "\n", 1); // Just write a newline without redisplaying the prompt
        rl_replace_line("", 0); // Clear the current line
        rl_on_new_line(); // Move to a new line
        rl_redisplay();

}

void	restore_terminal_settings(void)
{
	if (tcsetattr(STDIN_FILENO, TCSANOW, &g_orig_term) == -1)
	{
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}

void	add_to_history(const char *command)
{
	add_history(command);
}

void	setup_terminal(struct termios *term)
{
	if (tcgetattr(STDIN_FILENO, term) == -1)
	{
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}
	g_orig_term = *term;
	term->c_lflag &= ~(ECHOCTL);
	if (tcsetattr(STDIN_FILENO, TCSANOW, term) == -1)
	{
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}

void handle_signals(void) {
    signal(SIGQUIT, sigquit_handler);
    signal(SIGINT, SIG_IGN);
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, SIG_IGN); // Add this line
}

// ******************             main              ****************************

int	handle_command_input(char *command)
{
	if (command == NULL)
	{
		write(STDOUT_FILENO, "Exiting...\n", 11);
		return (1);
	}
	return (0);
}

void	reset_pipe_fds(t_command *cmd)
{
	t_command	*current_cmd;

	current_cmd = cmd;
	while (current_cmd)
	{
		current_cmd->pipe_fd[0] = -1;
		current_cmd->pipe_fd[1] = -1;
		current_cmd = current_cmd->next;
	}
}

/*/
void	free_command(t_command *cmd)
{
	t_command	*next_cmd;
	int			i;

	while (cmd)
	{
		next_cmd = cmd->next;
		i = 0;
		while (cmd->arguments[i])
		{
			free(cmd->arguments[i]);
			i++;
		}
		free(cmd->arguments);
		free(cmd->command);
		free(cmd);
		cmd = next_cmd;
	}
}
//*/

int	is_empty_command(char *command)
{
	if (command[0] == '\0')
	{
		free(command);
		return (1);
	}
	return (0);
}

t_command	*parse_command(char *command)
{
	t_command	*cmd;

	cmd = parse_line(command);
	if (cmd == NULL)
	{
		write(STDERR_FILENO, "Failed to parse command\n", 24);
	}
	return (cmd);
}

void free_command_fields(t_command *command) {
    if (command->command) {
        free(command->command);
        command->command = NULL;
    }
}

void free_arguments(t_command *command) {
    if (command->arguments) {
        for (int i = 0; i <= 64; i++) {
            if (command->arguments[i] != NULL) {
                free(command->arguments[i]);
                command->arguments[i] = NULL;
            }
        }
        free(command->arguments);
        command->arguments = NULL;
    }
}

void free_files(t_command *command) {
    if (command->input_file) {
        free(command->input_file);
        command->input_file = NULL;
    }
    if (command->output_file) {
        free(command->output_file);
        command->output_file = NULL;
    }
    if (command->append_file) {
        free(command->append_file);
        command->append_file = NULL;
    }
}

void free_heredoc(t_command *command) {
    if (command->heredoc_delim) {
        free(command->heredoc_delim);
        command->heredoc_delim = NULL;
    }
    if (command->close_heredoc_delim) {
        free(command->close_heredoc_delim);
        command->close_heredoc_delim = NULL;
    }
    if (command->heredoc_content) {
        free(command->heredoc_content);
        command->heredoc_content = NULL;
    }
}

void free_environment_variables(t_command *command) {
    if (command->environment_variables) {
        for (int i = 0; i <= 64; i++) {
            if (command->environment_variables[i] != NULL) {
                free(command->environment_variables[i]);
                command->environment_variables[i] = NULL;
            }
        }
        free(command->environment_variables);
        command->environment_variables = NULL;
    }
}

void free_command(t_command *command) {
    if (command == NULL) {
        return;
    }

    free_command_fields(command);
    free_arguments(command);
    free_files(command);
    free_heredoc(command);
    free_environment_variables(command);

    free(command);
}

void free_command_list(t_command *head) {
    t_command *current;
    t_command *next;

    current = head;
    while (current != NULL) {
        next = current->next;
        free_command(current);
        current = next;
    }
}

int	execute_and_reset(t_command *cmd)
{
	int	status;
	t_TokenizerState state;
	
	if (state.in_quotes == true)
		return (0);
	else
	{	
		status = execute_command(cmd);
		reset_pipe_fds(cmd);
		free_command_list(cmd);
	}
	return (status);
}

void	execute_loop(void)
{
	char		*command;
	int exit_status;
	t_command	*cmd;
	int in_single_quotes = 0;
    	int in_double_quotes = 0;

	in_single_quotes = 0;
	in_double_quotes = 0;
	exit_status = 0;
	while (1)
	{
		command = readline("minishell> ");
		if (handle_command_input(command))
			break ;
		if (is_empty_command(command))
			continue ;
			
			        // Check for unclosed quotes using a while loop and pointer arithmetic
        char *ptr = command;
         while (*ptr != '\0') {
            if (*ptr == '\'')
                in_single_quotes = !in_single_quotes;
            else if (*ptr == '"')
                in_double_quotes = !in_double_quotes;
            ptr++;
        }
        
                // If in_quotes is true, prompt for more input to close the quotes
         while (in_single_quotes || in_double_quotes) {
            char *more_input = readline("> ");
            char *new_command = malloc(strlen(command) + strlen(more_input) + 2);
            if (!new_command) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strcpy(new_command, command);
            strcat(new_command, "\n");
            strcat(new_command, more_input);
            free(command);
            command = new_command;
            free(more_input);

            // Check again for unclosed quotes
            in_single_quotes = 0;
            in_double_quotes = 0;
            ptr = command;
            while (*ptr != '\0') {
                if (*ptr == '\'')
                    in_single_quotes = !in_single_quotes;
                else if (*ptr == '"')
                    in_double_quotes = !in_double_quotes;
                ptr++;
            }
        }
        
		add_to_history(command);
		cmd = parse_command(command);
		free(command);		
		if (cmd == NULL)
			continue ;
		exit_status = execute_and_reset(cmd);
		if (exit_status == 1)
			break; // Exit the loop if g_exit_status is set to 1
	}
}

int	main(void)
{
    struct termios term;
        int exit_status;

	handle_signals();
	setup_terminal(&g_orig_term);
	execute_loop();
	restore_terminal_settings();
	return exit_status; // Return the exit status
}
