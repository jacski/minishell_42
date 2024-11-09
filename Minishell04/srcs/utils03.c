/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils03.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 19:54:52 by jkalinow          #+#    #+#             */
/*   Updated: 2024/10/27 19:54:55 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

void	memmove_reverse_copy(unsigned char *d, const unsigned char *s, \
	size_t n)
{
	while (n > 0)
	{
		n--;
		d[n] = s[n];
	}
}
