/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   death_and_gluttony.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:16:47 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 14:20:08 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	lonely_death(t_philos *p)
{
	pthread_mutex_lock(p->info->forks[p->id]);
	pthread_mutex_lock(p->info->print_mutex);
	printf("[%zu] %d has taken a fork 🥄\n",
		get_current_time() - p->init, p->id + 1);
	pthread_mutex_unlock(p->info->print_mutex);
	ft_newsleep(p->info->time_to_die);
	pthread_mutex_unlock(p->info->forks[p->id]);
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
					get_current_time() - p->init, p->id + 1);
			return (1);
		}
		pthread_mutex_unlock(p->eat_mutex);
	}
	return (0);
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
