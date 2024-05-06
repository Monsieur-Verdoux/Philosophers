/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 20:17:16 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/06 18:34:16 by akovalev         ###   ########.fr       */
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
	i = 0;
	while (info->forks[i])
	{
		pthread_mutex_destroy(info->forks[i]);
		free(info->forks[i]);
		//printf("Deforkage %zu successful\n", i);
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
		pthread_mutex_destroy(philos[i]->alive_mutex);
		pthread_mutex_destroy(philos[i]->eat_mutex);
		free(philos[i]->alive_mutex);
		philos[i]->alive_mutex = NULL;
		free(philos[i]->eat_mutex);
		philos[i]->eat_mutex = NULL;
		free(philos[i]);
		philos[i] = NULL;
		//printf("Dephilo %zu successful\n", i);
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

bool	potential_perishment(t_philos *p)
{
	pthread_mutex_lock(p->alive_mutex);
	if (!p->alive)
	{
		pthread_mutex_unlock(p->alive_mutex);
		return (1);
	}
	else
	{
		pthread_mutex_lock(p->eat_mutex);
		p->since_last_meal = get_current_time() - p->last_meal;
		if (p->since_last_meal > p->info->time_to_die)
		{
			//printf("slm is %zu and ttd is %zu\n", p->since_last_meal, p->info->time_to_die);
			pthread_mutex_unlock(p->eat_mutex);
			p->alive = 0;
			pthread_mutex_unlock(p->alive_mutex);
			pthread_mutex_lock(p->info->print);
			printf("[%zu] %d died 😵: I see you know your Judo well!\n", get_current_time() - p->init, p->id);
			pthread_mutex_unlock(p->info->print);
			return (1);
		}
		pthread_mutex_unlock(p->eat_mutex);
		pthread_mutex_unlock(p->alive_mutex);
	}
	return (0);
}

bool	equip_forks(t_philos *p)
{
	//printf("current philo is %d and his right fork is %d\n", p->id, p->right);
	pthread_mutex_lock(p->info->forks[p->right]);
	if (potential_perishment(p))
	{
		pthread_mutex_unlock(p->info->forks[p->right]);
		return (NULL);
	}
	pthread_mutex_lock(p->info->print);
	printf("[%zu] %d has taken a fork %d 🥄:\n",
		get_current_time() - p->init, p->id, p->right);
	pthread_mutex_unlock(p->info->print);
	pthread_mutex_lock(p->info->forks[p->id]);
	if (potential_perishment(p))
	{
		pthread_mutex_unlock(p->info->forks[p->id]);
		return (NULL);
	}
	pthread_mutex_lock(p->info->print);
	printf("[%zu] %d has taken a fork %d 🥄:\n",
		get_current_time() - p->init, p->id, p->id);
	pthread_mutex_unlock(p->info->print);
	return (p);
}

void	thinkage(t_philos *p)
{
	pthread_mutex_lock(p->info->print);
	printf("[%zu] %d is thinking 🤔: Gentlemen! This.. is.. Democracy.. Manifest..\n",
		get_current_time() - p->init, p->id);
	pthread_mutex_unlock(p->info->print);
	//p->status = THINK;
}



void	feedage(t_philos *p)
{
	pthread_mutex_lock(p->eat_mutex);
	p->last_meal = get_current_time();
	//printf("Philo %d ate at %zu\n", p->id, p->last_meal);
	p->meal_count++;
	pthread_mutex_unlock(p->eat_mutex);
	pthread_mutex_lock(p->info->print);
	printf("[%zu] %d is eating 🍽️: A succulent Chinese meal!\n", get_current_time() - p->init, p->id);
	pthread_mutex_unlock(p->info->print);
	ft_newsleep(p->info->time_to_eat);
	pthread_mutex_unlock(p->info->forks[p->right]);
	pthread_mutex_unlock(p->info->forks[p->id]);
}

void	sleepage(t_philos *p)
{
	pthread_mutex_lock(p->info->print);
	printf("[%zu] %d is sleeping 😴: Ta ta and farewell!\n", get_current_time() - p->init, p->id);
	pthread_mutex_unlock(p->info->print);
	//p->status = SLEEP;
	ft_newsleep(p->info->time_to_sleep);
}

void	*philosophize(void *ptr)
{
	t_philos	*philo;

	philo = (t_philos *)ptr;
	if (philo->id % 2 == 0)
		ft_newsleep(2);
	if (philo->info->number_of_philosophers == 1)
	{
		//lonely_death(philo);
		return (NULL);
	}
	while (!potential_perishment(philo))//(philo->alive) //why can't I use just philo->alive?
	{
		if (!equip_forks(philo))
			return (NULL);
		feedage(philo);
		if (potential_perishment(philo))
		 	return (NULL);
		sleepage(philo);
		if (potential_perishment(philo))
		 	return (NULL);
		thinkage(philo);
	}
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
	info->print = ft_calloc(1, sizeof(pthread_mutex_t));
	if (!malloc_fail(info->print))
		return ;
	pthread_mutex_init(info->print, NULL);
	info->init = get_current_time();
	//printf("current time is %zu\n", info->init);
}
	
int	cradle_of_philosophy(t_info *info, t_philos **philos)
{
	size_t	i;

	i = 0;
	while (i < info->number_of_philosophers)
	{
		//printf("i is %zu and philo num is %zu\n", i, info->number_of_philosophers);
		philos[i] = ft_calloc(1, sizeof(t_philos));
		if (!philos[i])
		{
			free_all((void *)philos);
			return (0);
		}
		philos[i]->id = i;
		philos[i]->info = info;
		philos[i]->init = info->init;
		philos[i]->meal_count = 0;
		philos[i]->alive = 1;
		philos[i]->last_meal = philos[i]->init;
		philos[i]->since_last_meal = 0;
		philos[i]->alive_mutex = ft_calloc(1, sizeof(pthread_mutex_t));
		if (!malloc_fail(philos[i]->alive_mutex))
			return (0);
		philos[i]->eat_mutex = ft_calloc(1, sizeof(pthread_mutex_t));
		if (!malloc_fail(philos[i]->eat_mutex))
			return (0);
		pthread_mutex_init(philos[i]->alive_mutex, NULL);
		pthread_mutex_init(philos[i]->eat_mutex, NULL);
		if (i == info->number_of_philosophers - 1)
			philos[i]->right = 0;
		else
			philos[i]->right = i + 1;
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
		//printf("i is %zu and philo num is %zu\n", i, info->number_of_philosophers);
		info->forks[i] = ft_calloc(1, sizeof(pthread_mutex_t));
		if (!malloc_fail(info->forks[i]))
			return (0);
		pthread_mutex_init(info->forks[i], NULL);
		//printf("Forkage %zu successful\n", i);
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

void	forced_perishment(t_philos **philos)
{
	int	i;

	i = 0;
	while (philos[i])
	{
		pthread_mutex_lock(philos[i]->alive_mutex);
		philos[i]->alive = 0;
		pthread_mutex_unlock(philos[i]->alive_mutex);
		i++;
	}
}

void	overseer(t_philos **philos)
{
	int	perishment;
	int	i;

	perishment = 0;
	i = 0;

	while (!perishment)
	{
		while (philos[i])
		{
			if (potential_perishment(philos[i]))
				perishment = 1;
			i++;
		}
		i = 0;
	}
	forced_perishment(philos);
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
	//ft_putstr_fd("Init successful\n", 1);
	philos = ft_calloc(info.number_of_philosophers + 1, sizeof(t_philos *));
	if (!malloc_fail(philos))
		return (0);
	//ft_putstr_fd("Before cradle\n", 1);
	forkage(&info);
	if (!cradle_of_philosophy(&info, philos))
		return (0);
	//ft_putstr_fd("Before forkage\n", 1);
	
	//ft_putstr_fd("Forkage successful\n", 1);
	overseer(philos);
	i = 0;
	while (philos[i])
	{
		pthread_join(philos[i]->thread, NULL);
		i++;
	}
	dephilosize(philos);
	demutexize(&info);
	//deforkage(&info);
	return (EXIT_SUCCESS);
}
