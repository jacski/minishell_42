/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal01.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:58:51 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 20:58:52 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

void	handle_signals(void)
{
	signal(SIGQUIT, sigquit_handler);
	signal(SIGINT, handle_sigint);
	signal(SIGTSTP, SIG_IGN);
}
