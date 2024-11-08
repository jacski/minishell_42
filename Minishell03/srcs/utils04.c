/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils04.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 19:57:12 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 19:57:16 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	*my_memmove(void *dest, const void *src, size_t n, size_t dest_size)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	if (n > dest_size)
	{
		write(STDERR_FILENO, "Error: destination buffer overflow risk\n", 40);
		return (NULL);
	}
	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	if (d > s)
		memmove_reverse_copy(d, s, n);
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

int	ft_atoi(const char *str)
{
	int	i;
	int	sign;
	int	nbr;

	i = 0;
	sign = 1;
	nbr = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-')
		sign = -1;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		nbr = (nbr * 10) + str[i] - '0';
		i++;
	}
	return (sign * nbr);
}

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

void	my_itoa(int num, char *str)
{
	int	i;

	i = 0;
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return ;
	}
	if (num < 0)
	{
		str[i++] = '-';
		num = -num;
	}
	while (num != 0)
	{
		str[i++] = (num % 10) + '0';
		num /= 10;
	}
	str[i] = '\0';
	reverse_string(str, i);
}
