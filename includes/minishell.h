/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 15:27:44 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/01 15:28:07 by jkalinow         ###   ########.fr       */
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
	char	quote_char;
}	t_TokenizerState;

typedef struct Command
{
	char			*command;
	char			**arguments;
	char			*input_file;
	char			*output_file;
	char			*append_file;
	int				input_fd;
	int				output_fd;
	int				pipe_fd[2];
	int				executed_successfully;
	char			**environment_variables;
	struct Command	*next;
}	t_command;

int			ft_strlen(char *str);
int			check_command_null(t_command *cmd);
int			handle_input_redirection_initialize_c(t_command *cmd);
int			create_output_file(t_command *cmd);
int			append_output_file(t_command *cmd);
void		set_environment_variables(t_command *cmd);
int			initialize_command(t_command *cmd);
char		*ft_strchr(const char *str, int character);
void		*ft_memset(void *ptr, int value, size_t len);
char		*ft_strcpy(char *s1, char *s2);
char		*ft_strdup(const char *str);
int			ft_strcmp(const char *s1, const char *s2);
size_t		my_strspn(const char *s1, const char *s2);
bool		entering_quoted_string(t_TokenizerState *state);
bool		exiting_quoted_string(t_TokenizerState *state);
bool		handle_delimiters(t_TokenizerState *state, const char *delim);
char		*handle_quoted_strings(t_TokenizerState *state, const char *delim);
char		*handle_double_redirection(t_TokenizerState *state);
char		*handle_single_redirection(t_TokenizerState *state);
char		*handle_non_token_redirection(t_TokenizerState *state, char *token);
char		*handle_specific_redirection(t_TokenizerState *state);
char		*handle_redirection_strtok(t_TokenizerState *state, char *token);
bool		handle_special_characters(t_TokenizerState *state, char **token);
char		*initialize_token(t_TokenizerState *state, char *str, \
		const char *delim);
char		*handle_pipe(t_TokenizerState *state);
char		*my_strtok(char *str, const char *delim);
char		*expand_variables(char *token);
void		expand_all_arguments(t_command *head);
char		*remove_quotes(char *str);
t_command	*create_command(int max_args);
void		handle_pipeline(t_command **current, int *arg_index, int max_args);
void		handle_redirection_parse_l(t_command *current, char **token);
void		handle_arguments(t_command *current, char *token, int *arg_index, \
		int max_args);
void		process_tokens(t_command **head, t_command **current, char *line, \
		int max_args);
t_command	*parse_line(char *line);
int			is_builtin(t_command *cmd);
int			handle_input_redirection_builtin(t_command *cmd, int *saved_stdin);
int			open_output_file(const char *file, int flags, \
		const char *error_message);
void		redirect_output(int output_fd, int *saved_stdout);
int			handle_output_redirection(t_command *cmd, int *saved_stdout);
int			execute_cd(t_command *cmd);
int			execute_echo(t_command *cmd);
int			handle_pwd(void);
int			handle_export(t_command *cmd);
int			handle_unset(t_command *cmd);
int			handle_env(void);
int			handle_exit(void);
int			execute_builtin_command(t_command *cmd);
void		restore_std(int saved_stdin, int saved_stdout);
int			handle_redirection_builtin(t_command *cmd, int *saved_stdin, \
		int *saved_stdout);
int			execute_builtin(t_command *cmd);
int			setup_pipes(t_command *cmd);
int			check_command(t_command *cmd);
pid_t		create_child_process(void);
void		handle_redirection_process(t_command *cmd);
void		execute_command_process(t_command *cmd);
int			wait_for_child_process(pid_t pid);
int			create_process(t_command *cmd);
int			create_pipe(int pipe_fd[2]);
void		exec_command(t_command *cmd);
int			setup_input_redirection(t_command *cmd, int *in_fd);
int			setup_pipe_output(int pipe_fd[2]);
int			setup_file_output(t_command *cmd);
int			setup_redirections(t_command *cmd, int in_fd, int pipe_fd[2]);
int			handle_fork(t_command *cmd, int in_fd, int pipe_fd[2]);
int			process_command(t_command *cmd, int *in_fd);
int			execute_pipeline(t_command *cmd);
int			execute_command(t_command *cmd);
void		handle_sigint(int sig);
void		restore_terminal_settings(void);
void		add_to_history(const char *command);
int			handle_command_input(char *command);
void		setup_terminal(struct termios *term);
void		handle_signals(void);
void		reset_pipe_fds(t_command *cmd);
void		free_command(t_command *cmd);
int			is_empty_command(char *command);
t_command	*parse_command(char *command);
int			execute_and_reset(t_command *cmd);
void		execute_loop(void);
int			main(void);

#endif
