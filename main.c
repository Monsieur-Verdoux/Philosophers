/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 20:17:16 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/03 18:25:53 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

#include <stdio.h>

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

bool	malloc_fail(void *p)
{
	if (!p)
	{
		ft_putstr_fd("malloc failure\n", 2);
		return (0);
	}
	return (1);
}

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

void	free_all(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		arr[i] = NULL;
		i++;
	}
	free(arr);
}


void	*ft_calloc(size_t count, size_t size)
{
	char	*ptr;
	size_t	i;
	size_t	max;

	max = 4611686014132420608;
	if (count != 0 && max / count < size)
		return (NULL);
	i = 0;
	ptr = malloc(count * size);
	if (ptr == NULL)
		return (NULL);
	while (i < count * size)
	{
		*(ptr + i) = '\0';
		i++;
	}
	return (ptr);
}

static int	atoi_putnbr(const char *str, int sign, int i)
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
		{
			if (sign == 1)
				return (-1);
			else if (sign == -1)
				return (0);
		}
		result = (result * 10) + (str[i] - '0');
		i++;
	}
	return (sign * (int)result);
}

int	ft_atoi(const char *str)
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

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	else
		return (0);
}

int	input_validation(char **argv)
{
	int	i;
	int	j;

	i = 1;
	j = 0;
	if (ft_atoi(argv[1]) == 0)
	{
		ft_putstr_fd("At least one thinker is required\n", 2);
		return (1);
	}
	while (argv[i])
	{
		while (argv[i][j])
		{
			if (!ft_isdigit(argv[i][j]))
			{
				ft_putstr_fd("Arguments must only contain digits 0-9\n", 2);
				return (1);
			}
			j++;
		}
		j = 0;
		i++;
	}
	return (0);
}
void	*philosophize(void *ptr)
{
	t_philos	*philos;

	philos = (t_philos *)ptr;
	printf("Gentlemen! This.. is.. Democracy.. Manifest.. from seat %zu\n", philos->id);
	return (NULL);
}

void	initialize(char **argv, int argc, t_info *info)
{
	info->number_of_philosophers = ft_atoi(argv[1]);
	info->time_to_die = ft_atoi(argv[2]);
	info->time_to_eat = ft_atoi(argv[3]);
	info->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		info->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
	else
		info->number_of_times_each_philosopher_must_eat = 0; //maybe need to handle 0 being passed as argument separately from not having it at all
}
int	cradle_of_philosophy(t_info *info, t_philos **philos)
{
	size_t	i;

	i = 0;
	while (i < info->number_of_philosophers)
	{
		philos[i] = ft_calloc(1, sizeof(t_philos));
		if (!philos[i])
		{
			free_all((void *)philos);
			return (0);
		}
		philos[i]->id = i;
		philos[i]->info = info;
		pthread_create(&philos[i]->thread, NULL, philosophize, (void *)philos[i]);
		i++;
	}
	return (1);
}

int	forkage(t_info *info)
{
	size_t	i;

	info->forks = ft_calloc(info->number_of_philosophers + 1, sizeof(pthread_mutex_t *));
	i = 0;
	while (i < info->number_of_philosophers)
	{
		info->forks[i] = ft_calloc(1, sizeof(pthread_mutex_t));
		if (!malloc_fail(info->forks[i]))
			return (0);
		pthread_mutex_init(info->forks[i], NULL);
		printf("Forkage %zu successful\n", i);
		i++;
	}
	return (1);
}
void	deforkage(t_info *info)
{
	size_t	i;

	i = 0;
	while (info->forks[i])
	{
		pthread_mutex_destroy(info->forks[i]);
		free(info->forks[i]);
		info->forks[i] = NULL;
		printf("Deforkage %zu successful\n", i);
		i++;
	}
	free(info->forks);
	info->forks = NULL;
}

int	main(int argc, char **argv)
{
	t_info		info;
	t_philos	**philos;
	int			i;

	if (argc != 5 && argc != 6)
	{
		ft_putstr_fd ("Please provide the correct number of arguments\n", 2);
		return (EXIT_FAILURE);
	}
	if (input_validation(argv))
		return (EXIT_FAILURE);
	else
		ft_putstr_fd("Arguments validated\n", 1);
	initialize(argv, argc, &info);
	philos = ft_calloc(info.number_of_philosophers + 1, sizeof(t_philos *));
	if (!malloc_fail(philos))
		return (0);
	if (!cradle_of_philosophy(&info, philos))
		return (0);
	forkage(&info);
	i = 0;
	while (philos[i])
	{
		pthread_join(philos[i]->thread, NULL);
		i++;
	}
	deforkage(&info);
	return (EXIT_SUCCESS);
}