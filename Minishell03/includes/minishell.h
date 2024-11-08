/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 19:05:58 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 19:07:29 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

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
extern struct termios	g_orig_term;

typedef struct TokenizerState
{
	char	*saveptr;
	bool	in_quotes;
	bool	in_single_quotes;
	char	quote_char;
	char	heredoc_delim[256];
	bool	is_heredoc_mark;
	bool	prev_heredoc;
}	t_TokenizerState;

typedef struct Command
{
	char			*command;
	char			**arguments;
	bool			*expand_flags;
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
	int				exit_status;
	char			**environment_variables;
	struct Command	*next;
	bool			is_heredoc_mark;
	bool			prev_heredoc;
	bool			next_heredoc;
	char			*heredoc_content;
}	t_command;

typedef struct s_parse_context
{
	t_command			*head;
	t_command			*current;
	t_TokenizerState	*state;
	int					max_args;
	int					arg_index;
}	t_parse_context;

typedef struct s_input_state
{
	int		in_single_quotes;
	int		in_double_quotes;
	int		heredoc_active;
	char	*heredoc_delim;
}	t_input_state;

typedef struct s_command_context
{
	int				*exit_status;
	char			**heredoc_delim;
	t_input_state	*input_state;
}	t_command_context;

int				ft_strlen(char *str);
int				ft_isalnum(int c);
char			*ft_strchr(const char *str, int character);
void			*ft_memset(void *ptr, int value, size_t len);
char			*ft_strcpy(char *s1, char *s2);
char			*my_strncpy(char *dest, const char *src, size_t n);
char			*ft_strdup(const char *str);
char			*ft_strndup(const char *s1, size_t n);
int				ft_strcmp(const char *s1, const char *s2);
int				my_strncmp(const char *str1, const char *str2, size_t n);
size_t			my_strspn(const char *s1, const char *s2);
char			*my_strstr(const char *haystack, const char *needle);
char			*my_strcat(char *dest, const char *src);
void			memmove_reverse_copy(unsigned char *d, const unsigned char *s, \
				size_t n);
void			*my_memcpy(void *dest, const void *src, size_t n);
void			*my_memmove(void *dest, const void *src, size_t n, size_t \
				dest_size);
char			*ft_strtok(char *str, const char *delim);
int				ft_atoi(const char *str);
void			reverse_string(char *str, int length);
void			my_itoa(int num, char *str);
char			*initialize_token(t_parse_context *context, char *str, \
					const char *delim);
bool			entering_quoted_string(t_parse_context *context);
bool			exiting_quoted_string(t_parse_context *context);
bool			handle_delimiters(t_parse_context *context, const char *delim);
char			*handle_pipe(t_parse_context *context);
char			*handle_non_token_redirection(t_parse_context *context, \
					char *token);
char			*handle_double_redirection(t_parse_context *context);
char			*handle_single_redirection(t_parse_context *context);
char			*handle_specific_redirection(t_parse_context *context);
char			*handle_redirection_strtok(t_parse_context *context, \
					char *token);
bool			is_heredoc_delimiter(t_parse_context *context);
char			*process_heredoc_delimiter(t_parse_context *context);
char			*process_heredoc_content(t_parse_context *context);
bool			process_quoted_string(t_parse_context *context);
char			*finalize_token(t_parse_context *context, char *token);
bool			handle_special_characters(t_parse_context *context, \
					char **token);
bool			has_unclosed_quote(t_TokenizerState *state);
char			*handle_quoted_strings(t_parse_context *context, \
					const char *delim);
char			*my_strtok(t_parse_context *context, char *str, \
					const char *delim);
char			*expand_variables(char *token, t_command *cmd);
void			expand_all_arguments(t_command *head);
char			*remove_quotes(char *str);
void			init_command(t_command *cmd);
char			**allocate_arguments(int max_args, t_command *cmd);
t_command		*create_command(t_command *cmd);
void			handle_pipeline(t_command **current, int *arg_index);
void			handle_input_redirection(t_command *current, char *token, \
					t_parse_context *context);
void			handle_output_redirection_parsel(t_command *current, \
					char *token, t_parse_context *context);
void			handle_append_redirection(t_command *current, char *token, \
					t_parse_context *context);
void			handle_heredoc_redirection(t_command *current, char *token, \
					t_parse_context *context);
void			handle_redirection_parse_l(t_command *current, char *token, \
					t_parse_context *context);
void			handle_too_many_arguments(t_command *current);
void			handle_heredoc(t_command *current, char *token);
void			handle_input_file(t_command *current, char *token);
void			handle_output_file(t_command *current, char *token);
void			handle_append_file(t_command *current, char *token);
void			handle_next_heredoc(t_command *current, char *token);
void			handle_file_redirection_logic(t_command *cmd, \
					char *unquoted_token, char *token, \
					t_parse_context *context);
void			handle_heredoc_logic(t_command *cmd, char *token);
void			handle_argument_overflow_and_redirection(t_command *cmd, \
					t_parse_context *context, char *unquoted_token, \
					char *token);
void			handle_arguments(t_parse_context *context, char *token);
void			initialize_command_context(t_parse_context *context);
int				process_token(t_parse_context *context, char *token);
void			process_tokens(t_parse_context *context, char *line);
t_command		*parse_line(char *line, t_command *cmd, \
					t_parse_context *context);
void			free_command_fields(t_command *command);
void			free_arguments(t_command *command);
void			free_files(t_command *command);
void			free_heredoc(t_command *command);
void			free_environment_variables(t_command *command);
void			free_command(t_command *command);
void			free_command_list(t_command *head);
void			free_command_at_end(t_command *command, \
					t_parse_context *context);
int				handle_input_redirection_builtin(t_command *cmd, \
					int *saved_stdin);
int				open_output_file(const char *file, int flags, \
					const char *error_message);
void			redirect_output(int output_fd, int *saved_stdout);
int				handle_output_redirection(t_command *cmd, int *saved_stdout);
int				execute_cd(t_command *cmd);
int				execute_echo(t_command *cmd);
int				handle_pwd(void);
void			add_new_environment_variable(t_command *cmd, char *new_var, \
					int *var_count);
char			*create_new_variable(char *key, char *value);
int				update_environment_variable(t_command *cmd, char *key, \
					char *value, int *var_count);
int				process_export_argument(t_command *cmd, char *arg, \
					int *var_count);
int				count_environment_variables(t_command *cmd);
int				handle_export(t_command *cmd);
void			shift_environment_variables(t_command *cmd, int start_index);
void			unset_variable(t_command *cmd, char *argument, int env_count);
int				handle_unset(t_command *cmd);
int				handle_env(t_command *cmd);
int				handle_exit(t_command *cmd, t_parse_context *context);
int				is_builtin(t_command *cmd);
int				execute_builtin_command(t_command *cmd, \
					t_parse_context *context);
void			restore_std(int saved_stdin, int saved_stdout);
int				handle_redirection_builtin(t_command *cmd, int *saved_stdin, \
					int *saved_stdout);
int				execute_builtin(t_command *cmd, t_parse_context *context);
int				setup_input_redirection(t_command *cmd, int *in_fd);
int				setup_pipe_output(int pipe_fd[2]);
int				setup_file_output(t_command *cmd);
int				setup_pipes(t_command *cmd);
int				setup_redirections(t_command *cmd, int in_fd, int pipe_fd[2]);
int				handle_and_exec_command(t_command *cmd, int in_fd, \
					int pipe_fd[2]);
int				create_pipe(int pipe_fd[2]);
int				process_command(t_command *cmd, int *in_fd);
int				execute_pipeline(t_command *cmd);
int				check_command_null(t_command *cmd);
int				handle_input_redirection_initialize_c(t_command *cmd);
int				create_output_file(t_command *cmd);
int				append_output_file(t_command *cmd);
int				initialize_command(t_command *cmd);
void			process_dollar_question_replacement(char *pos, char *buffer, \
				size_t buffer_len);
void			expand_dollar_question(char *input, t_command *cmd);
int				check_command(t_command *cmd);
pid_t			create_child_process(void);
void			handle_heredoc_exc(t_command *cmd);
void			handle_input_redirection_exc(t_command *cmd);
void			handle_output_redirection_exc(t_command *cmd);
void			handle_redirection_process(t_command *cmd);
void			execute_command_process(t_command *cmd);
int				wait_for_child_process(pid_t pid);
char			*check_absolute_path(const char *cmd);
char			*get_env_variable_value(t_command *cmd, const char *var_name);
char			*duplicate_path_env(t_command *cmd);
char			*build_full_path(const char *dir, const char *cmd);
char			*iterate_directories(char *dir, const char *cmd);
char			*find_in_directories(char *paths, const char *cmd);
char			*find_command_in_path(const char *cmd, \
					t_command *command_struct);
char			*get_command_path(t_command *cmd);
int				fork_and_exec(t_command *cmd, char *command_path);
int				handle_process_exit_status(int status, t_command *cmd);
int				create_process(t_command *cmd);
int				handle_regular_command(t_command *cmd);
int				execute_command(t_command *cmd, t_parse_context *context);
void			sigquit_handler(int sig);
void			handle_sigint(int sig);
void			handle_eof(int sig);
void			restore_terminal_settings(void);
void			add_to_history(const char *command);
void			setup_terminal(struct termios *term);
void			handle_signals(void);
int				handle_command_input(char *command);
void			reset_pipe_fds(t_command *cmd);
t_command		*parse_command(char *command, t_command *cmd, \
					t_parse_context *context);
void			reset_command_strings(t_command *cmd);
void			free_command_list_reset(t_command *cmd);
void			reset_command_fields(t_command *cmd);
void			reset_command_and_free_next(t_command *cmd);
int				execute_and_reset(t_command *cmd, t_parse_context *context);
char			*combine_command(char *command, char *more_input);
void			check_quotes(char *command, t_input_state *input_state);
char			*extract_delimiter(char *ptr);
int				check_line_match(char *line_start, char *line_end, \
					t_input_state *input_state);
int				check_remaining_input(char *line_start, \
					t_input_state *input_state);
int				is_heredoc_closed(char *input, t_input_state *input_state);
int				check_heredoc_delimiter(char *ptr, t_input_state *input_state);
int				detect_heredoc(char *command, t_input_state *input_state);
int				handle_input_loop(char **command, t_input_state *input_state);
int				process_input(char **command, t_input_state *input_state);
void			free_command_and_delim(char **command, char **heredoc_delim);
int				handle_empty_command(char **command);
int				handle_command_inputs(char **command, \
					t_input_state *input_state);
int				process_input_and_expand(char **command, t_input_state \
					*input_state, t_command *cmd);
int				parse_and_execute_command(char **command, t_command *cmd, \
					t_parse_context *context);
int				process_command_loop(char **command, \
					t_input_state *input_state, t_command *cmd, \
					t_parse_context *context);
char			*get_command(void);
void			copy_environment_variables(t_command *cmd);
t_command		*initialize_command_state(t_input_state *input_state);
void			execute_loop(t_input_state *input_state, t_command *cmd, \
					t_parse_context *context);
t_parse_context	*initialize_parse_context(void);
int				main(void);

#endif
