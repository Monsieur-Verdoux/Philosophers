/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:05:52 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 14:07:41 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	ft_putchar_fd(char c, int fd)
{
	write (fd, &c, 1);
}

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	if (s == NULL)
		return ;
	i = 0;
	while (s[i] != '\0')
	{
		ft_putchar_fd(s[i], fd);
		i++;
	}
	return ;
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;
	size_t	malloc_size;

	malloc_size = count * size;
	if (count != 0 && malloc_size / count != size)
		return (NULL);
	ptr = (malloc(malloc_size));
	if (ptr == NULL)
		return (NULL);
	memset(ptr, 0, malloc_size);
	return (ptr);
}

bool	check_malloc_fail(void *p)
{
	if (!p)
	{
		ft_putstr_fd("Malloc failure\n", 2);
		return (0);
	}
	return (1);
}

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	else
		return (0);
}
