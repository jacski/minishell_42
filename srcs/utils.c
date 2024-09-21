/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 16:55:14 by jkalinow          #+#    #+#             */
/*   Updated: 2024/09/01 16:55:20 by jkalinow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
