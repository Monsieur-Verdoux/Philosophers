/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:08:46 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 14:10:15 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		ft_putstr_fd("gettimeofday() error\n", 2);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

int	ft_newsleep(size_t milliseconds)
{
	size_t	start;

	start = get_current_time();
	while (get_current_time() - start < milliseconds)
		usleep(500);
	return (0);
}

static long	atoi_putnbr(const char *str, int sign, int i)
{
	long	result;

	result = 0;
	if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (result >= LONG_MAX / 10 && (result > LONG_MAX / 10 \
		|| (str[i] - '0') > LONG_MAX % 10))
			return (2147483648);
		result = (result * 10) + (str[i] - '0');
		i++;
	}
	return (sign * result);
}

long	ft_atoi_long(const char *str)
{
	int		i;
	int		sign;

	i = 0;
	sign = 1;
	while ((str[i] == '\t') || (str[i] == '\n') || (str[i] == '\v')
		|| (str[i] == '\f') || (str[i] == '\r') || (str[i] == ' '))
		i++;
	return (atoi_putnbr(str, sign, i));
}
