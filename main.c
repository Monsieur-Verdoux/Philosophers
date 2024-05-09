/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 20:17:16 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/09 18:43:13 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

#include <stdio.h>

void	demutexize(t_info *info)
{
	int	i;

	pthread_mutex_destroy(info->print);
	free(info->print);
	info->print = NULL;
	pthread_mutex_destroy(info->print_mutex);
	free(info->print_mutex);
	info->print_mutex = NULL;
	pthread_mutex_destroy(info->start_mutex);
	free(info->start_mutex);
	info->start_mutex = NULL;
	i = 0;
	while (info->forks[i])
	{
		pthread_mutex_destroy(info->forks[i]);
		free(info->forks[i]);
		info->forks[i] = NULL;
		i++;
	}
	free(info->forks);
	info->forks = NULL;
}

void	dephilosize(t_philos **philos)
{
	int	i;

	i = 0;
	while (philos[i])
	{
		pthread_mutex_destroy(philos[i]->eat_mutex);
		free(philos[i]->eat_mutex);
		philos[i]->eat_mutex = NULL;
		free(philos[i]);
		philos[i] = NULL;
		i++;
	}
	free(philos);
	philos = NULL;
}

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

bool	check_malloc_fail(void *p)
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

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	else
		return (0);
}

int	check_characters(char **argv)
{
	int	i;
	int	j;

	i = 1;
	j = 0;
	while (argv[i])
	{
		while (argv[i][j])
		{
			if (!ft_isdigit(argv[i][j]))
			{
				ft_putstr_fd("Arguments must only contain digits 0-9\n", 2);
				return (0);
			}
			j++;
		}
		j = 0;
		i++;
	}
	return (1);
}

int	input_validation(char **argv, int argc)
{
	int	i;

	i = 1;
	if (argc != 5 && argc != 6)
	{
		ft_putstr_fd ("Please provide the correct number of arguments\n", 2);
		return (0);
	}
	if (!check_characters(argv))
		return (0);
	while (argv[i])
	{
		if (ft_atoi_long(argv[i]) > 2147483647)
		{
			ft_putstr_fd("Only integers are allowed as arguments\n", 2);
			return (0);
		}
		i++;
	}
	if (ft_atoi_long(argv[1]) == 0)
	{
		ft_putstr_fd("At least one thinker is required\n", 2);
		return (0);
	}
	return (1);
}

bool	register_death(t_philos *p)
{
	pthread_mutex_lock(p->info->print_mutex);
	if (!p->info->death_or_full)
	{
		p->info->death_or_full = 1;
		pthread_mutex_unlock(p->info->print_mutex);
		return (1);
	}
	pthread_mutex_unlock(p->info->print_mutex);
	return (0);
}

bool	potential_perishment(t_philos *p)
{
	pthread_mutex_lock(p->info->print_mutex);
	if (p->info->death_or_full)
	{
		pthread_mutex_unlock(p->info->print_mutex);
		return (1);
	}
	else
	{
		pthread_mutex_unlock(p->info->print_mutex);
		pthread_mutex_lock(p->eat_mutex);
		p->since_last_meal = get_current_time() - p->last_meal;
		if (p->since_last_meal > p->time_to_die)
		{
			pthread_mutex_unlock(p->eat_mutex);
			if (register_death(p))
				printf("[%zu] %d died 😵: I see you know your Judo well!\n", \
					get_current_time() - p->init, p->id);
			return (1);
		}
		pthread_mutex_unlock(p->eat_mutex);
	}
	return (0);
}

void	equip_forks(t_philos *p)
{
	pthread_mutex_lock(p->info->forks[p->right]);
	pthread_mutex_lock(p->info->print_mutex);
	if (p->info->death_or_full)
	{
		pthread_mutex_unlock(p->info->forks[p->right]);
		pthread_mutex_unlock(p->info->print_mutex);
		return ;
	}
	printf("[%zu] %d has taken a fork %d 🥄:\n", \
		get_current_time() - p->init, p->id, p->right);
	pthread_mutex_unlock(p->info->print_mutex);
	pthread_mutex_lock(p->info->forks[p->id]);
	pthread_mutex_lock(p->info->print_mutex);
	if (p->info->death_or_full)
	{
		pthread_mutex_unlock(p->info->forks[p->right]);
		pthread_mutex_unlock(p->info->forks[p->id]);
		pthread_mutex_unlock(p->info->print_mutex);
		return ;
	}
	printf("[%zu] %d has taken a fork %d 🥄:\n", \
			get_current_time() - p->init, p->id, p->id);
	pthread_mutex_unlock(p->info->print_mutex);
}

void	thinkage(t_philos *p)
{
	pthread_mutex_lock(p->info->print_mutex);
	if (!p->info->death_or_full)
	{
		printf("[%zu] %d is thinking 🤔:", get_current_time() - p->init, p->id);
		printf(" Gentlemen! This.. is.. Democracy.. Manifest..\n");
	}
	pthread_mutex_unlock(p->info->print_mutex);
}

void	feedage(t_philos *p)
{
	pthread_mutex_lock(p->eat_mutex);
	p->last_meal = get_current_time();
	p->meal_count++;
	pthread_mutex_lock(p->info->print_mutex);
	pthread_mutex_unlock(p->eat_mutex);
	if (!p->info->death_or_full)
		printf("[%zu] %d is eating 🍽️: A succulent Chinese meal!\n", \
			get_current_time() - p->init, p->id);
	pthread_mutex_unlock(p->info->print_mutex);
	while (get_current_time() - p->last_meal < p->time_to_eat)
		usleep(500);
	pthread_mutex_unlock(p->info->forks[p->right]);
	pthread_mutex_unlock(p->info->forks[p->id]);
}

void	sleepage(t_philos *p)
{
	size_t	sleep_start;

	sleep_start = get_current_time();
	pthread_mutex_lock(p->info->print_mutex);
	if (!p->info->death_or_full)
		printf("[%zu] %d is sleeping 😴: Ta ta and farewell!\n", \
			get_current_time() - p->init, p->id);
	pthread_mutex_unlock(p->info->print_mutex);
	while (get_current_time() - sleep_start < p->time_to_sleep)
		usleep (500);
}

void	lonely_death(t_philos *p)
{
	pthread_mutex_lock(p->info->forks[p->id]);
	pthread_mutex_lock(p->info->print_mutex);
	printf("[%zu] %d has taken a fork %d 🥄:\n",
		get_current_time() - p->init, p->id, p->id);
	pthread_mutex_unlock(p->info->print_mutex);
	ft_newsleep(p->info->time_to_die);
	pthread_mutex_unlock(p->info->forks[p->id]);
}

void	*philosophize(void *ptr)
{
	t_philos	*philo;

	philo = (t_philos *)ptr;
	pthread_mutex_lock(philo->info->start_mutex);
	philo->init = philo->info->init;
	philo->last_meal = philo->init;
	pthread_mutex_unlock(philo->info->start_mutex);
	if (philo->id % 2 == 0)
	{
		thinkage(philo);
		ft_newsleep(10);
	}
	if (philo->info->number_of_philosophers == 1)
		lonely_death(philo);
	while (!potential_perishment(philo))
	{
		equip_forks(philo);
		feedage(philo);
		sleepage(philo);
		thinkage(philo);
	}
	return (NULL);
}

int	initialize(char **argv, int argc, t_info *info)
{
	info->number_of_philosophers = ft_atoi_long(argv[1]);
	info->time_to_die = ft_atoi_long(argv[2]);
	info->time_to_eat = ft_atoi_long(argv[3]);
	info->time_to_sleep = ft_atoi_long(argv[4]);
	if (argc == 6)
		info->number_of_times_each_philosopher_must_eat = ft_atoi_long(argv[5]);
	else
		info->number_of_times_each_philosopher_must_eat = 0;
	info->print = ft_calloc(1, sizeof(pthread_mutex_t));
	if (!check_malloc_fail(info->print))
		return (0);
	pthread_mutex_init(info->print, NULL);
	info->print_mutex = ft_calloc(1, sizeof(pthread_mutex_t));
	if (!check_malloc_fail(info->print_mutex))
		return (0);
	pthread_mutex_init(info->print_mutex, NULL);
	info->start_mutex = ft_calloc(1, sizeof(pthread_mutex_t));
	if (!check_malloc_fail(info->start_mutex))
		return (0);
	pthread_mutex_init(info->start_mutex, NULL);
	info->death_or_full = 0;
	return (1);
}

void	birth_a_philo(t_info *info, t_philos **philos, size_t i)
{
	philos[i]->id = i;
	philos[i]->time_to_die = info->time_to_die;
	philos[i]->time_to_eat = info->time_to_eat;
	philos[i]->time_to_sleep = info->time_to_sleep;
	philos[i]->number_of_philosophers = info->number_of_philosophers;
	philos[i]->number_of_times_each_philosopher_must_eat = \
		info->number_of_times_each_philosopher_must_eat;
	philos[i]->info = info;
	philos[i]->meal_count = 0;
	philos[i]->since_last_meal = 0;
	if (i == info->number_of_philosophers - 1)
		philos[i]->right = 0;
	else
		philos[i]->right = i + 1;
}

int	cradle_of_philosophy(t_info *info, t_philos **philos)
{
	size_t	i;

	i = 0;
	while (i < info->number_of_philosophers)
	{
		philos[i] = ft_calloc(1, sizeof(t_philos));
		if (!check_malloc_fail(philos[i]))
			return (0);
		birth_a_philo(info, philos, i);
		philos[i]->eat_mutex = ft_calloc(1, sizeof(pthread_mutex_t));
		if (!check_malloc_fail(philos[i]->eat_mutex))
			return (0);
		pthread_mutex_init(philos[i]->eat_mutex, NULL);
		pthread_create(&philos[i]->thread, NULL, \
			philosophize, (void *)philos[i]);
		i++;
	}
	philos[i] = NULL;
	return (1);
}

int	forkage(t_info *info)
{
	size_t	i;

	info->forks = ft_calloc(info->number_of_philosophers + 1, \
		sizeof(pthread_mutex_t *));
	if (!check_malloc_fail(info->forks))
	{
		demutexize(info);
		return (0);
	}
	i = 0;
	while (i < info->number_of_philosophers)
	{
		info->forks[i] = ft_calloc(1, sizeof(pthread_mutex_t));
		if (!check_malloc_fail(info->forks[i]))
		{
			demutexize(info);
			return (0);
		}
		pthread_mutex_init(info->forks[i], NULL);
		i++;
	}
	info->forks[i] = NULL;
	return (1);
}

bool	successful_gluttony(t_philos **philos)
{
	int	i;

	i = 0;
	if (!philos[0]->number_of_times_each_philosopher_must_eat)
		return (0);
	while (philos[i])
	{
		pthread_mutex_lock(philos[i]->eat_mutex);
		if (philos[i]->meal_count < \
			philos[i]->number_of_times_each_philosopher_must_eat)
		{
			pthread_mutex_unlock(philos[i]->eat_mutex);
			return (0);
		}
		pthread_mutex_unlock(philos[i]->eat_mutex);
		i++;
	}
	pthread_mutex_lock(philos[0]->info->print_mutex);
	philos[0]->info->death_or_full = 1;
	pthread_mutex_unlock(philos[0]->info->print_mutex);
	return (1);
}

void	overseer(t_philos **philos)
{
	int	perishment;
	int	i;
	int	gluttony;

	gluttony = 0;
	perishment = 0;
	i = 0;
	ft_newsleep(10);
	while (!perishment && !gluttony)
	{
		while (philos[i])
		{
			if (potential_perishment(philos[i]))
				perishment = 1;
			if (successful_gluttony(philos))
				gluttony = 1;
			i++;
		}
		i = 0;
	}
}

void	cleanup(t_info *info, t_philos **philos)
{
	int	i;

	i = 0;
	while (philos[i])
	{
		pthread_join(philos[i]->thread, NULL);
		i++;
	}
	dephilosize(philos);
	demutexize(info);
}

int	main(int argc, char **argv)
{
	t_info		info;
	t_philos	**philos;

	if (!input_validation(argv, argc) || \
		!initialize(argv, argc, &info) || !forkage(&info))
		return (EXIT_FAILURE);
	philos = ft_calloc(info.number_of_philosophers + 1, sizeof(t_philos *));
	if (!check_malloc_fail(philos))
	{
		demutexize(&info);
		return (EXIT_FAILURE);
	}
	pthread_mutex_lock(info.start_mutex);
	if (!cradle_of_philosophy(&info, philos))
	{
		demutexize(&info);
		dephilosize(philos);
		return (EXIT_FAILURE);
	}
	info.init = get_current_time();
	pthread_mutex_unlock(info.start_mutex);
	overseer(philos);
	cleanup(&info, philos);
	return (EXIT_SUCCESS);
}
