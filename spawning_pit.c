/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawning_pit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:23:17 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 14:30:21 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	birthe_a_philo(t_info *info, t_philos **philos, size_t i)
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
		birthe_a_philo(info, philos, i);
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

int	forge_forks(t_info *info)
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
