// update 09/10/2024 14:50
// to do memory leaks
// to do norminette
// -lreadline

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

typedef struct s_input_state
{
    int in_single_quotes;
    int in_double_quotes;
    int heredoc_active;
    char *heredoc_delim;
} t_input_state;

typedef struct s_operands {
    int left;
    int right;
} t_operands;

/*/ ******************   check funtion   ******************************
void	print_command_structure(t_command *cmd)
{
	int i;

	while (cmd != NULL)
	{
		printf("Command: \n");
//		if (cmd->command)
//			printf("Command: %s\n", cmd->command);
		printf("Arguments:\n");
		printf("cmd->prev_input_file:%d\n", cmd->prev_input_file);
//		if (cmd->arguments)
//		{
//			printf("Arguments:\n");
//			for (i = 0; cmd->arguments[i] != NULL; i++)
//			{
//				printf("\t[%d]: *%s*\n", i, cmd->arguments[i]);
//			}
//		}

		// Print file redirections
		printf("Input file: \n");
//		if (cmd->input_file)
//			printf("Input file: %s\n", cmd->input_file);
		printf("Output file: \n");
//		if (cmd->output_file)
//			printf("Output file: %s\n", cmd->output_file);
		printf("Append file: \n");
//		if (cmd->append_file)
//			printf("Append file: %s\n", cmd->append_file);

		// Print heredoc information
		printf("heredoc_delim: \n");
//		if (cmd->heredoc_delim)
//			printf("heredoc_delim: %s\n", cmd->heredoc_delim);
		printf("heredoc_content: \n");
//		if (cmd->heredoc_content)
//			printf("heredoc_content: %s\n", cmd->heredoc_content);
		printf("close_heredoc_delim: \n");
//		if (cmd->close_heredoc_delim)
//			printf("close_heredoc_delim: %s\n", cmd->close_heredoc_delim);

		// Print file descriptors (if you're using them)
		printf("Input FD: %d\n", cmd->input_fd);
		printf("Output FD: %d\n", cmd->output_fd);

		// Print pipe info (if any)
		printf("Pipe FD: \n");
//		if (cmd->pipe_fd[0] != -1 && cmd->pipe_fd[1] != -1)
//			printf("Pipe FD: [%d, %d]\n", cmd->pipe_fd[0], cmd->pipe_fd[1]);

		// Check if the command executed successfully
		printf("Executed successfully: %d\n", cmd->executed_successfully);

		// Move to the next command in the pipeline (if any)
		cmd = cmd->next;
	}
	printf("Command_structure_end: \n");
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

char	*my_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
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

int	my_strncmp(const char *str1, const char *str2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
			return ((unsigned char)str1[i] - (unsigned char)str2[i]);
		i++;
	}
	if (i < n)
		return ((unsigned char)str1[i] - (unsigned char)str2[i]);
	return (0);
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

char	*my_strstr(const char *haystack, const char *needle)
{
	const char	*h;
	const char	*n;

	if (*needle == '\0')
		return ((char *)haystack);
	while (*haystack != '\0')
	{
		h = haystack;
		n = needle;
		while (*h == *n && *n != '\0')
		{
			h++;
			n++;
		}
		if (*n == '\0')
			return ((char *)haystack);
		haystack++;
	}
	return (NULL);
}

char	*my_strcat(char *dest, const char *src)
{
	char	*dest_ptr;

	dest_ptr = dest;
	while (*dest_ptr != '\0')
		dest_ptr++;
	while (*src != '\0')
	{
		*dest_ptr = *src;
		dest_ptr++;
		src++;
	}
	*dest_ptr = '\0';
	return (dest);
}

void	*my_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dest);
}

void	*my_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	if (d > s)
	{
		while (n > 0)
		{
			n--;
			d[n] = s[n];
		}
	}
	else
	{
		i = 0;
		while (i < n)
		{
			d[i] = s[i];
			i++;
		}
	}
	return (dest);
}

char	*ft_strtok(char *str, const char *delim)
{
	static char	*saveptr;
	char		*token;

	if (str != NULL)
		saveptr = str;
	if (saveptr == NULL)
		return (NULL);
	while (*saveptr && strchr(delim, *saveptr))
		saveptr++;
	if (*saveptr == '\0')
		return (NULL);
	token = saveptr;
	while (*saveptr && !strchr(delim, *saveptr))
		saveptr++;
	if (*saveptr)
		*saveptr++ = '\0';
	return (token);
}

int ft_atoi(const char *str)
{
	int i = 0;
	int sign = 1;
	int nbr = 0;

	while(str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if(str[i] == '-')
		sign = -1;
	if(str[i] == '-' || str[i] == '+')
		i++;
	while(str[i] >= '0' && str[i] <= '9')
	{
		nbr = (nbr * 10) + str[i] - '0';
		i++;
	}
	return (sign * nbr);
}

/*/
void my_itoa(int value, char *str) {
    sprintf(str, "%d", value);  // Use sprintf or another safe way to convert int to string
}
//*/

//*/

void	reverse_string(char *str, int length)
{
	int		start;
	int		end;
	char	temp;

	start = 0;
	end = length - 1;
	while (start < end)
	{
		temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}

void	append_digits(int *i, int num, char *str)
{
	while (num != 0)
	{
		str[(*i)++] = (num % 10) + '0';
		num /= 10;
	}
}

int	handle_negative(int *i, int num, char *str)
{
	str[(*i)++] = '-';
	return (-num);
}

int	handle_zero_case(int num, int *i, char *str)
{
	if (num == 0)
	{
		str[(*i)++] = '0';
		str[*i] = '\0';
		return (1);
	}
	return (0);
}

void	my_itoa(int num, char *str)
{
	int	i;

	i = 0;
	if (handle_zero_case(num, &i, str))
		return ;
	if (num < 0)
		num = handle_negative(&i, num, str);
	append_digits(&i, num, str);
	str[i] = '\0';
	reverse_string(str, i);
}
//*/

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
	if (!state->in_quotes && ft_strchr(delim, *state->saveptr))
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
	my_strncpy(separ_spec_char, token, len);
	separ_spec_char[len] = '\0';
	result = ft_strdup(separ_spec_char);
	free(separ_spec_char);
	return (result);
}

char	*handle_double_redirection(t_TokenizerState *state)
{
	char	*saveptr_backup;
	int		i;

	if (*state->saveptr == '<' && *(state->saveptr + 1) == '<')
	{
		saveptr_backup = state->saveptr + 2;
		state->saveptr += 2;
		while (*state->saveptr == ' ' || *state->saveptr == '\t')
			state->saveptr++;
		i = 0;
		while (!ft_strchr(" \t\n\0", state->saveptr[i]))
			i++;
		my_strncpy(state->heredoc_delim, state->saveptr, i);
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
	if (ft_strchr("<>", *state->saveptr) != NULL)
	{
		*token = handle_redirection_strtok(state, *token);
		return (true);
	}
	return (false);
}

bool	is_heredoc_delimiter(t_TokenizerState *state)
{
	size_t	heredoc_delim_len;
	char	*ptr;

	heredoc_delim_len = ft_strlen(state->heredoc_delim);
	ptr = state->saveptr;
	if (my_strncmp(ptr, state->heredoc_delim, heredoc_delim_len) == 0)
	{
		if (*(ptr + heredoc_delim_len) == '\n' || *(ptr + heredoc_delim_len) \
			== '\0')
		{
			ptr--;
			while (ptr > state->saveptr && ft_strchr(" \t", *ptr))
				ptr--;
			if (ptr == state->saveptr || *ptr == '\n')
				return (true);
		}
	}
	return (false);
}

char	*process_heredoc_content(t_TokenizerState *state)
{
	char	*token;

	token = state->saveptr;
	while (*state->saveptr)
	{
		if (is_heredoc_delimiter(state))
		{
			if (*(state->saveptr - 1) == '\n')
				*(state->saveptr - 1) = '\0';
			state->prev_heredoc = false;
			break ;
		}
		state->saveptr++;
	}
	return (token);
}

bool	process_quoted_string(t_TokenizerState *state)
{
	if (entering_quoted_string(state))
		return (true);
	if (exiting_quoted_string(state))
		return (true);
	return (false);
}

size_t	get_var_name_len(const char *str)
{
	size_t	len;

	len = 0;
	str++;
	while (*str && (ft_isalnum(*str) || *str == '_'))
	{
		len++;
		str++;
	}
	return (len);
}

void	copy_suffix(char *new_token, size_t prefix_len, const char *var_value, \
		t_TokenizerState *state)
{
	const char	*suffix;

	suffix = state->saveptr + prefix_len + get_var_name_len(state->saveptr);
	ft_strcpy(new_token + prefix_len + ft_strlen((char *)var_value), \
		(char *)suffix);
}

void	copy_var_value(char *new_token, size_t prefix_len, \
		const char *var_value)
{
	ft_strcpy(new_token + prefix_len, (char *)var_value);
}

void	copy_prefix(char *new_token, const char *saveptr, size_t prefix_len)
{
	my_strncpy(new_token, saveptr, prefix_len);
}

size_t	calculate_prefix_len(t_TokenizerState *state)
{
	char	*dollar_pos;

	dollar_pos = ft_strchr(state->saveptr, '$');
	return (dollar_pos - state->saveptr);
}

char	*allocate_new_token(size_t new_len)
{
	char	*new_token;

	new_token = malloc(new_len + 1);
	if (!new_token)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	return (new_token);
}

size_t	calculate_new_length(t_TokenizerState *state, const char *var_value)
{
	size_t	token_len;
	size_t	var_value_len;

	token_len = ft_strlen(state->saveptr);
	var_value_len = ft_strlen((char *)var_value);
	return (token_len - get_var_name_len(state->saveptr) + var_value_len);
}

void	replace_with_env_value(t_TokenizerState *state, const char *var_value)
{
	char	*new_token;
	size_t	new_len;
	size_t	prefix_len;

	new_len = calculate_new_length(state, var_value);
	new_token = allocate_new_token(new_len);
	prefix_len = calculate_prefix_len(state);
	copy_prefix(new_token, state->saveptr, prefix_len);
	copy_var_value(new_token, prefix_len, var_value);
	copy_suffix(new_token, prefix_len, var_value, state);
	state->saveptr = new_token;
}

void	process_env_variable(t_TokenizerState *state)
{
	char	*start;
	char	*var_value;
	char	var_name[256];
	int		i;

	start = state->saveptr + 1;
	i = 0;
	while (*start && (ft_isalnum(*start) || *start == '_'))
		var_name[i++] = *start++;
	var_name[i] = '\0';
	var_value = getenv(var_name);
	if (var_value)
		replace_with_env_value(state, var_value);
	state->saveptr = start;
}

char	*finalize_token(t_TokenizerState *state, char *token)
{
	if (state->saveptr == NULL)
		return (NULL);
	if (ft_strcmp(state->heredoc_delim, token) == 0)
		state->prev_heredoc = true;
	return (token);
}

bool	process_env_variable_if_needed(t_TokenizerState *state)
{
	if (state->in_quotes && state->quote_char == '"' && *state->saveptr == '$')
	{
		process_env_variable(state);
		return (true);
	}
	return (false);
}

bool	handle_special_characters_if_needed(t_TokenizerState *state, \
		char **token)
{
	if (ft_strchr("<>|", *state->saveptr) && !state->in_quotes)
		return (handle_special_characters(state, token));
	return (false);
}

char	*handle_quoted_strings(t_TokenizerState *state, const char *delim)
{
	char	*token;

	token = state->saveptr;
	while (*state->saveptr)
	{
		if (process_quoted_string(state))
			continue ;
		if (state->prev_heredoc)
			return (process_heredoc_content(state));
		if (handle_special_characters_if_needed(state, &token))
			return (token);
		if (process_env_variable_if_needed(state))
			continue ;
		if (handle_delimiters(state, delim) && !state->prev_heredoc)
			break ;
		state->saveptr++;
	}
	return (finalize_token(state, token));
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
			return (ft_strdup(var_value));
		else
			return (ft_strdup(""));
	}
	return (ft_strdup(token));
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
			if (current->arguments[i] != NULL)
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
	cmd->prev_heredoc = false;
	cmd->next_heredoc = false;
	cmd->environment_variables = NULL;
	cmd->next = NULL;
}

char	**allocate_arguments(int max_args)
{
	char	**arguments;
	int		i;

	arguments = malloc(sizeof(char *) * (max_args + 1));
	if (!arguments)
	{
		perror("malloc");
		return (NULL);
	}
	i = 0;
	while (i <= max_args)
	{
		arguments[i] = NULL;
		i++;
	}
	return (arguments);
}

char	**allocate_environment_variables(void)
{
	char	**env_vars;
	int		i;

	env_vars = malloc(sizeof(char *) * 65);
	if (!env_vars)
	{
		perror("malloc");
		return (NULL);
	}
	i = 0;
	while (i < 65)
	{
		env_vars[i] = NULL;
		i++;
	}
	return (env_vars);
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
	init_command(cmd);
	cmd->arguments = allocate_arguments(max_args);
	if (!cmd->arguments)
	{
		free(cmd);
		exit(EXIT_FAILURE);
	}
	cmd->environment_variables = allocate_environment_variables();
	if (!cmd->environment_variables)
	{
		free(cmd->arguments);
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

void	handle_input_redirection(t_command *current, char **token, \
		t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_input_file = true;
}

void	handle_output_redirection_parsel(t_command *current, \
		char **token, t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_output_file = true;
}

void	handle_append_redirection(t_command *current, char **token, \
		t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_append_file = true;
}

void	handle_heredoc_redirection(t_command *current, char **token, \
		t_TokenizerState *state)
{
	*token = my_strtok(state, NULL, " \t\n");
	if (*token)
		current->prev_heredoc = true;
}

void	handle_redirection_parse_l(t_command *current, char **token)
{
	t_TokenizerState	state;

	state.saveptr = *token;
	if (ft_strcmp(*token, "<") == 0)
		handle_input_redirection(current, token, &state);
	else if (ft_strcmp(*token, ">") == 0)
		handle_output_redirection_parsel(current, token, &state);
	else if (ft_strcmp(*token, ">>") == 0)
		handle_append_redirection(current, token, &state);
	else if (ft_strcmp(*token, "<<") == 0)
		handle_heredoc_redirection(current, token, &state);
}

void	handle_too_many_arguments(t_command *current)
{
	write(2, "Too many arguments for command: \n", 33);
	write(2, current->command, ft_strlen(current->command));
	write(2, "\n", 1);
}

void	handle_heredoc(t_command *current, char *token)
{
	if (current->heredoc_delim && current->heredoc_content == NULL)
	{
		if (token != NULL)
		{
			current->heredoc_content = ft_strdup(token);
			current->prev_heredoc = false;
		}
		current->next_heredoc = true;
	}
	if (current->heredoc_delim == NULL)
		current->heredoc_delim = ft_strdup(token);
}

void	handle_input_file(t_command *current, char *token)
{
	if (current->prev_input_file)
	{
		if (token != NULL)
		{
			current->input_file = ft_strdup(token);
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
			current->output_file = ft_strdup(token);
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
			current->append_file = ft_strdup(token);
			current->prev_append_file = false;
		}
	}
}

void	handle_next_heredoc(t_command *current, char *token)
{
	if (token != NULL)
	{
		if (current->close_heredoc_delim != NULL)
			free(current->close_heredoc_delim);
		current->close_heredoc_delim = ft_strdup(token);
		current->next_heredoc = false;
	}
}

void	handle_command_arg(t_command *current, char *unquoted_token)
{
	if (current->command != NULL)
		free(current->command);
	current->command = ft_strdup(unquoted_token);
}

void	handle_argument(t_command *current, char *unquoted_token, \
		int *arg_index, int max_args)
{
	if (current->arguments[*arg_index] != NULL)
		free(current->arguments[*arg_index]);
	current->arguments[(*arg_index)++] = ft_strdup(unquoted_token);
}

void	handle_arguments(t_command *current, char *token, int *arg_index, \
		int max_args)
{
	char	*unquoted_token;

	unquoted_token = remove_quotes(token);
	if (*arg_index == 0)
		handle_command_arg(current, unquoted_token);
	if (*arg_index >= max_args - 1)
	{
		handle_too_many_arguments(current);
		free(unquoted_token);
		return ;
	}
	if (current->prev_heredoc)
		handle_heredoc(current, token);
	else if (!current->next_heredoc && !current->prev_input_file && \
		!current->prev_output_file && !current->prev_append_file)
		handle_argument(current, unquoted_token, arg_index, max_args);
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
	if (ft_strcmp(token, "|") == 0)
		handle_pipeline(&context->current, &context->arg_index, \
			context->max_args);
	else if (ft_strcmp(token, "<") == 0 || ft_strcmp(token, ">") == 0 || \
		ft_strcmp(token, ">>") == 0 || ft_strcmp(token, "<<") == 0)
		handle_redirection_parse_l(context->current, &token);
	else
		handle_arguments(context->current, token, &context->arg_index, \
			context->max_args);
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
	t_parse_context		context;
	t_TokenizerState	state;

	state.saveptr = NULL;
	state.in_quotes = false;
	state.quote_char = '\0';
	state.heredoc_delim[0] = '\0';
	state.prev_heredoc = false;
	context.head = NULL;
	context.current = NULL;
	context.max_args = 64;
	context.arg_index = 0;
	context.state = &state;
	process_tokens(&context, line);
	if (state.in_quotes)
		write(2, "Error: Unclosed quote at the end of input.\n", 43);
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
	if (cmd->arguments[1] && ft_strcmp(cmd->arguments[1], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (cmd->arguments[i])
	{
		write(1, cmd->arguments[i], ft_strlen(cmd->arguments[i]));
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
	{
		write(STDOUT_FILENO, cwd, ft_strlen(cwd));
		write(STDOUT_FILENO, "\n", 1);
	}
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
		env = ft_strchr(cmd->arguments[i], '=');
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
		if (ft_strcmp(cmd->command, builtins[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

int	execute_builtin_command(t_command *cmd)
{
	if (ft_strcmp(cmd->command, "cd") == 0)
		return (execute_cd(cmd));
	else if (ft_strcmp(cmd->command, "exit") == 0)
		return (handle_exit());
	else if (ft_strcmp(cmd->command, "echo") == 0)
		return (execute_echo(cmd));
	else if (ft_strcmp(cmd->command, "pwd") == 0)
		return (handle_pwd());
	else if (ft_strcmp(cmd->command, "export") == 0)
		return (handle_export(cmd));
	else if (ft_strcmp(cmd->command, "unset") == 0)
		return (handle_unset(cmd));
	else if (ft_strcmp(cmd->command, "env") == 0)
		return (handle_env());
	else
	{
		write(STDERR_FILENO, "Unknown built-in command: /n", 27);
		write(STDERR_FILENO, cmd->command, ft_strlen(cmd->command));
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
	int			last_status;

	current_cmd = cmd;
	in_fd = 0;
	last_status = 0;
	while (current_cmd != NULL)
	{
		if (process_command(current_cmd, &in_fd) == -1)
			return (-1);
		current_cmd = current_cmd->next;
	}
	while (wait(&status) > 0)
		last_status = status;
	return ((last_status >> 8) & 0xFF);
}

// ******************        initialize_command       **************************

int	check_command_null(t_command *cmd)
{
	const char	*error_msg;

	error_msg = "Command is NULL\n";
	if (cmd == NULL)
	{
		write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
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
			write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
			write(STDERR_FILENO, cmd->input_file, \
				ft_strlen(cmd->input_file));
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
		write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
		write(STDERR_FILENO, cmd->output_file, ft_strlen(cmd->output_file));
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
		write(STDERR_FILENO, error_msg, ft_strlen((char *)error_msg));
		write(STDERR_FILENO, cmd->append_file, ft_strlen(cmd->append_file));
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

char	*strip_spaces(char *str)
{
	char	*result;
	char	*ptr;

	result = malloc(ft_strlen(str) + 1);
	ptr = result;
	if (!result)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	while (*str)
	{
		if (*str != ' ' && *str != '\t' && *str != '\n')
			*ptr++ = *str;
		str++;
	}
	*ptr = '\0';
	return (result);
}

char	detect_operator(char *clean_expr, char **operator_pos)
{
	*operator_pos = ft_strchr(clean_expr, '+');
	if (*operator_pos != NULL)
		return ('+');
	*operator_pos = ft_strchr(clean_expr, '*');
	if (*operator_pos != NULL)
		return ('*');
	*operator_pos =ft_strchr(clean_expr, '-');
	if (*operator_pos != NULL)
		return ('-');
	*operator_pos = ft_strchr(clean_expr, '/');
	if (*operator_pos != NULL)
		return ('/');
	return ('\0');
}

int	parse_operands(char *clean_expr, char *operator_pos, int *left, int *right)
{
	char	*left_operand;
	char	*right_operand;
	char	*left_stripped;
	char	*right_stripped;

	*operator_pos = '\0';
	left_operand = clean_expr;
	right_operand = operator_pos + 1;
	left_stripped = strip_spaces(left_operand);
	right_stripped = strip_spaces(right_operand);
	*left = ft_atoi(left_stripped);
	*right = ft_atoi(right_stripped);
	free(left_stripped);
	free(right_stripped);
	if (*right == 0 && *operator_pos == '/')
		return (-1);
	return (0);
}

int	calculate_result(int left, int right, char operator)
{
	if (operator == '+')
		return (left + right);
	else if (operator == '-')
		return (left - right);
	else if (operator == '*')
		return (left * right);
	else if (operator == '/')
	{
		if (right != 0)
			return (left / right);
		else
		{
			write(STDERR_FILENO, "Division by zero error\n", 23);
			return (-1);
		}
	}
	else
	{
		write(STDERR_FILENO, "Unknown operator\n", 17);
		return (-1);
	}
}

int	evaluate_expr(char *expr)
{
	char		*clean_expr;
	char		*operator_pos;
	char		operator;
	int			result;
	t_operands	operands;

	clean_expr = strip_spaces(expr);
	operator_pos = NULL;
	operator = detect_operator(clean_expr, &operator_pos);
	if (operator == '\0')
	{
		write(STDERR_FILENO, "No valid operator found in expression\n", 38);
		free(clean_expr);
		return (-1);
	}
	if (parse_operands(clean_expr, operator_pos, &operands.left, \
		&operands.right) == -1)
	{
		write(STDERR_FILENO, "Division by zero error\n", 23);
		free(clean_expr);
		return (-1);
	}
	result = calculate_result(operands.left, operands.right, operator);
	free(clean_expr);
	return (result);
}

void	expand_dollar_question(char *input, int exit_status)
{
	char	buffer[12];
	char	*pos;

	pos = input;
	my_itoa(exit_status, buffer);
	pos = my_strstr(pos, "$?");
	while (pos != NULL)
	{
		my_memmove(pos + ft_strlen(buffer), pos + 2, ft_strlen(pos + 2) + 1);
		my_memcpy(pos, buffer, ft_strlen(buffer));
		pos += ft_strlen(buffer);
		pos = my_strstr(pos, "$?");
	}
}

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

void	handle_heredoc_exc(t_command *cmd)
{
	int	heredoc_pipe[2];

	if (pipe(heredoc_pipe) == -1)
	{
		write(STDERR_FILENO, "Error creating heredoc pipe\n", 28);
		exit(EXIT_FAILURE);
	}
	write(heredoc_pipe[1], cmd->heredoc_content, ft_strlen(cmd->heredoc_content));
	write(heredoc_pipe[1], "\n", 1);
	close(heredoc_pipe[1]);
	dup2(heredoc_pipe[0], STDIN_FILENO);
	close(heredoc_pipe[0]);
}

void	handle_input_redirection_exc(t_command *cmd)
{
	if (cmd->input_fd != -1)
	{
		dup2(cmd->input_fd, STDIN_FILENO);
		close(cmd->input_fd);
	}
}

void	handle_output_redirection_exc(t_command *cmd)
{
	if (cmd->output_fd != -1)
	{
		dup2(cmd->output_fd, STDOUT_FILENO);
		close(cmd->output_fd);
	}
}

void	handle_redirection_process(t_command *cmd)
{
	if (cmd->heredoc_delim != NULL && ft_strcmp(cmd->heredoc_delim, \
		cmd->close_heredoc_delim) == 0)
		handle_heredoc_exc(cmd);
	handle_input_redirection_exc(cmd);
	handle_output_redirection_exc(cmd);
}


void	execute_command_process(t_command *cmd)
{
	if (execvp(cmd->command, cmd->arguments) == -1)
	{
		write(STDERR_FILENO, "Error executing command: ", 25);
		write(STDERR_FILENO, cmd->command, ft_strlen(cmd->command));
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

char	*check_absolute_path(const char *cmd)
{
	if (cmd[0] == '/')
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	return (NULL);
}

char	*duplicate_path_env(void)
{
	char	*path;

	path = getenv("PATH");
	if (!path || ft_strlen(path) == 0)
		return (NULL);
	return (ft_strdup(path));
}

char	*build_full_path(const char *dir, const char *cmd)
{
	size_t	dir_len;
	size_t	cmd_len;
	char	*full_path;

	dir_len = ft_strlen((char *)dir);
	cmd_len = ft_strlen((char *)cmd);
	full_path = malloc(dir_len + cmd_len + 2);
	if (!full_path)
	{
		perror("malloc");
		return (NULL);
	}
	ft_strcpy(full_path, (char *)dir);
	my_strcat(full_path, "/");
	my_strcat(full_path, cmd);
	return (full_path);
}

char	*find_in_directories(char *paths, const char *cmd)
{
	char	*dir;
	char	*full_path;

	dir = ft_strtok(paths, ":");
	full_path = NULL;
	while (dir != NULL)
	{
		full_path = build_full_path(dir, cmd);
		if (!full_path)
		{
			free(paths);
			return (NULL);
		}
		if (access(full_path, X_OK) == 0)
		{
			free(paths);
			return (full_path);
		}
		free(full_path);
		dir = ft_strtok(NULL, ":");
	}
	return (NULL);
}

char	*find_command_in_path(const char *cmd)
{
	char	*command;
	char	*paths;

	command = check_absolute_path(cmd);
	if (command)
		return (command);
	paths = duplicate_path_env();
	if (!paths)
		return (NULL);
	command = find_in_directories(paths, cmd);
	free(paths);
	return (command);
}

char	*get_command_path(t_command *cmd, int *exit_status)
{
	char	*command_path;

	command_path = find_command_in_path(cmd->command);
	if (!command_path)
	{
		write(STDERR_FILENO, "Command not found or PATH is not set\n", 37);
		*exit_status = 127;
	}
	return (command_path);
}

int	fork_and_exec(t_command *cmd, char *command_path)
{
	extern char	**environ;
	pid_t		pid;

	pid = fork();
	if (pid == -1)
	{
		free(command_path);
		return (-1);
	}
	if (pid == 0)
	{
		handle_redirection_process(cmd);
		if (execve(command_path, cmd->arguments, environ) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
	}
	return (pid);
}

int	handle_process_exit_status(int status, int *exit_status, t_command *cmd)
{
	if ((status & 0x7F) == 0)
		*exit_status = (status >> 8) & 0xFF;
	else if ((status & 0x7F) != 0)
		*exit_status = 128 + (status & 0x7F);
	else
		*exit_status = (-1);
	if (status == 0)
		cmd->executed_successfully = 1;
	else
		cmd->executed_successfully = 0;
	return (0);
}

int	create_process(t_command *cmd, int *exit_status)
{
	char	*command_path;
	int		pid;
	int		status;

	command_path = get_command_path(cmd, exit_status);
	if (!command_path)
		return (-1);
	pid = fork_and_exec(cmd, command_path);
	if (pid == -1)
	{
		free(command_path);
		return (-1);
	}
	if (waitpid(pid, &status, 0) == -1)
	{
		free(command_path);
		return (-1);
	}
	free(command_path);
	return (handle_process_exit_status(status, exit_status, cmd));
}

/*/
int	handle_expr_command(t_command *cmd, int *exit_status)
{
//	char	expr_buffer[128] = {0};
	char	expr_buffer[128];	
	expr_buffer[0] = '\0';
	printf("cmd->arguments[1] %s\n", cmd->arguments[1]);
	printf("cmd->arguments[2] %s\n", cmd->arguments[2]);
	printf("cmd->arguments[3] %s\n", cmd->arguments[3]);
	if (cmd->arguments[1] && cmd->arguments[2] && cmd->arguments[3])
	{
		strncat(expr_buffer, cmd->arguments[1], sizeof(expr_buffer) - \
			ft_strlen(expr_buffer) - 1);
		printf("expr_buffer *%s*\n", expr_buffer);
		strncat(expr_buffer, " ", sizeof(expr_buffer) - \
			ft_strlen(expr_buffer) - 1);
		printf("expr_buffer *%s*\n", expr_buffer);
		strncat(expr_buffer, cmd->arguments[2], sizeof(expr_buffer) - \
			ft_strlen(expr_buffer) - 1);
		strncat(expr_buffer, " ", sizeof(expr_buffer) - \
			ft_strlen(expr_buffer) - 1);
		strncat(expr_buffer, cmd->arguments[3], sizeof(expr_buffer) - \
			ft_strlen(expr_buffer) - 1);
		printf("expr_buffer *%s*\n", expr_buffer);
		expand_dollar_question(expr_buffer, *exit_status);
		printf("expanded_expr_buffer *%s*\n", expr_buffer);
//		*exit_status = evaluate_expr(expr_buffer);
	}
	else
	{
		write(STDERR_FILENO, "Invalid expression\n", 19);
		return (-1);
	}
	printf("exit_status *%d*\n", *exit_status);
	return (create_process(cmd, exit_status));
}

//*/ int	handle_regular_command(t_command *cmd, int *exit_status)
{
	if (cmd->next)
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
		return (create_process(cmd, exit_status));
	}
}

int	execute_command(t_command *cmd, int *exit_status)
{
	int	result;

	if (is_builtin(cmd))
		result = execute_builtin(cmd);
	else
		result = handle_regular_command(cmd, exit_status);
	if (result != 0)
		return (result);
	return (result);
}


// ******************            signals            ****************************

void	sigquit_handler(int sig)
{
	(void) sig;
	rl_on_new_line();
	rl_redisplay();
}

void	handle_sigint(int sig)
{
	(void) sig;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
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

void	handle_signals(void)
{
	signal(SIGQUIT, sigquit_handler);
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, handle_sigint);
	signal(SIGTSTP, SIG_IGN);
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

void	free_command_fields(t_command *command)
{
	if (command->command)
	{
		free(command->command);
		command->command = NULL;
	}
}

void	free_arguments(t_command *command)
{
	int	i;

	if (command->arguments)
	{
		i = 0;
		while (i <= 64 && command->arguments[i] != NULL)
		{
			free(command->arguments[i]);
			command->arguments[i] = NULL;
			i++;
		}
		free(command->arguments);
		command->arguments = NULL;
	}
}

void	free_files(t_command *command)
{
	if (command->input_file)
	{
		free(command->input_file);
		command->input_file = NULL;
	}
	if (command->output_file)
	{
		free(command->output_file);
		command->output_file = NULL;
	}
	if (command->append_file)
	{
		free(command->append_file);
		command->append_file = NULL;
	}
}

void	free_heredoc(t_command *command)
{
	if (command->heredoc_delim)
	{
		free(command->heredoc_delim);
		command->heredoc_delim = NULL;
	}
	if (command->close_heredoc_delim)
	{
		free(command->close_heredoc_delim);
		command->close_heredoc_delim = NULL;
	}
	if (command->heredoc_content)
	{
		free(command->heredoc_content);
		command->heredoc_content = NULL;
	}
}

void	free_environment_variables(t_command *command)
{
	int	i;

	if (command->environment_variables)
	{
		i = 0;
		while (i <= 64)
		{
			if (command->environment_variables[i] != NULL)
			{
				free(command->environment_variables[i]);
				command->environment_variables[i] = NULL;
			}
			i++;
		}
		free(command->environment_variables);
		command->environment_variables = NULL;
	}
}

void	free_command(t_command *command)
{
	if (command == NULL)
		return ;
	free_command_fields(command);
	free_arguments(command);
	free_files(command);
	free_heredoc(command);
	free_environment_variables(command);
	free(command);
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
		current = next;
	}
}

int	execute_and_reset(t_command *cmd, int *exit_status)
{
	int	status;

	status = execute_command(cmd, exit_status);
	reset_pipe_fds(cmd);
	free_command_list(cmd);
	return (status);
}

char	*combine_command(char *command, char *more_input)
{
	char	*new_command;

	new_command = malloc(ft_strlen(command) + ft_strlen(more_input) + 2);
	if (!new_command)
	{
		perror("malloc");
		free(command);
		free(more_input);
		exit(EXIT_FAILURE);
	}
	ft_strcpy(new_command, command);
	my_strcat(new_command, "\n");
	my_strcat(new_command, more_input);
	free(command);
	free(more_input);
	return (new_command);
}

void	check_quotes(char *command, t_input_state *input_state)
{
	char	*ptr;

	ptr = command;
	while (*ptr != '\0')
	{
		if (*ptr == '\'')
			input_state->in_single_quotes = !input_state->in_single_quotes;
		else if (*ptr == '"')
			input_state->in_double_quotes = !input_state->in_double_quotes;
		ptr++;
	}
}

char	*skip_whitespace(char *ptr)
{
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')
		ptr++;
	return (ptr);
}

char	*extract_delimiter(char *ptr)
{
	char	*delim;
	char	*delim_end;

	delim = ft_strdup(ptr);
	delim_end = delim;
	while (*delim_end != '\0' && *delim_end != ' ' && *delim_end != '\n')
		delim_end++;
	*delim_end = '\0';
	return (delim);
}

int	check_heredoc_delimiter(char *ptr, t_input_state *input_state)
{
	ptr += 2;
	ptr = skip_whitespace(ptr);
	input_state->heredoc_delim = extract_delimiter(ptr);
	if (input_state->heredoc_delim[0] != '\0')
	{
		input_state->heredoc_active = 1;
		return (1);
	}
	return (0);
}

int	detect_heredoc(char *command, t_input_state *input_state)
{
	char	*ptr;

	ptr = command;
	while (*ptr != '\0')
	{
		if (my_strncmp(ptr, "<<", 2) == 0)
			if (check_heredoc_delimiter(ptr, input_state))
				return (1);
		ptr++;
	}
	return (0);
}

int	handle_input_loop(char **command, t_input_state *input_state)
{
	char	*more_input;

	while (input_state->in_single_quotes || input_state->in_double_quotes || \
		input_state->heredoc_active)
	{
		more_input = readline("> ");
		if (!more_input)
		{
			free(*command);
			return (1);
		}
		if (my_strncmp(input_state->heredoc_delim, more_input, \
			(size_t)ft_strlen(input_state->heredoc_delim)) == 0)
			input_state->heredoc_active = 0;
		check_quotes(*command, input_state);
		*command = combine_command(*command, more_input);
	}
	return (0);
}

int	process_input(char **command, t_input_state *input_state)
{
	input_state->heredoc_active = detect_heredoc(*command, \
		input_state);
	check_quotes(*command, input_state);
	return (handle_input_loop(command, input_state));
}

void	free_command_and_delim(char **command, char **heredoc_delim)
{
	if (*command)
	{
		free(*command);
		*command = NULL;
	}
	if (*heredoc_delim)
	{
		free(*heredoc_delim);
		*heredoc_delim = NULL;
	}
}

int	handle_empty_command(char **command)
{
	if (is_empty_command(*command))
	{
		free(*command);
		*command = NULL;
		return (1);
	}
	return (0);
}

int	process_input_and_expand(char **command, t_input_state *input_state, \
		int *exit_status)
{
	if (process_input(command, input_state))
	{
		free(*command);
		*command = NULL;
		return (1);
	}
	expand_dollar_question(*command, *exit_status);
	add_to_history(*command);
	return (0);
}

int	parse_and_execute_command(char **command, int *exit_status)
{
	t_command	*cmd;

	cmd = parse_command(*command);
	if (!cmd)
	{
		free(*command);
		*command = NULL;
		return (1);
	}
	execute_and_reset(cmd, exit_status);
	return (0);
}

int	process_command_loop(char **command, char **heredoc_delim, \
		int *exit_status, t_input_state *input_state)
{
	if (handle_empty_command(command))
		return (1);
	if (process_input_and_expand(command, input_state, exit_status))
		return (1);
	if (parse_and_execute_command(command, exit_status))
		return (1);
	free(*command);
	*command = NULL;
	return (0);
}

char	*get_command(void)
{
	char	*command;

	command = readline("minishell> ");
	if (!command)
	{
		handle_command_input(NULL);
		return (NULL);
	}
	if (handle_command_input(command))
	{
		free(command);
		return (NULL);
	}
	return (command);
}

void	execute_loop(void)
{
	char			*command;
	int				exit_status;
	t_input_state	input_state;

	input_state.in_single_quotes = 0;
	input_state.in_double_quotes = 0;
	input_state.heredoc_active = 0;
	input_state.heredoc_delim = NULL;
	exit_status = 0;
	command = NULL;
	while (1)
	{
		command = get_command();
		if (!command)
			break ;

		if (process_command_loop(&command, &input_state.heredoc_delim, \
			&exit_status, &input_state))
			continue ;
		free_command_and_delim(&command, &input_state.heredoc_delim);

	}
	if (input_state.heredoc_delim)
		free(input_state.heredoc_delim);
}

int	main(void)
{
	struct termios	term;
	int	exit_status;

	exit_status = 0;
	handle_signals();
	setup_terminal(&g_orig_term);
	execute_loop();
	restore_terminal_settings();
	return (exit_status);
}
