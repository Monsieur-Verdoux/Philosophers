/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophize.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:10:53 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 14:29:52 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	forkage(t_philos *p)
{
	pthread_mutex_lock(p->info->forks[p->right]);
	pthread_mutex_lock(p->info->print_mutex);
	if (p->info->death_or_full)
	{
		pthread_mutex_unlock(p->info->forks[p->right]);
		pthread_mutex_unlock(p->info->print_mutex);
		return ;
	}
	printf("[%zu] %d has taken a fork 🥄\n", \
		get_current_time() - p->init, p->id + 1);
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
	printf("[%zu] %d has taken a fork 🥄\n", \
			get_current_time() - p->init, p->id + 1);
	pthread_mutex_unlock(p->info->print_mutex);
}

void	thinkage(t_philos *p)
{
	pthread_mutex_lock(p->info->print_mutex);
	if (!p->info->death_or_full)
	{
		printf("[%zu] %d is thinking 🤔:", \
			get_current_time() - p->init, p->id + 1);
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
		printf("[%zu] %d is eating 🍽️ : A succulent Chinese meal!\n", \
			get_current_time() - p->init, p->id + 1);
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
			get_current_time() - p->init, p->id + 1);
	pthread_mutex_unlock(p->info->print_mutex);
	while (get_current_time() - sleep_start < p->time_to_sleep)
		usleep (500);
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
		forkage(philo);
		feedage(philo);
		sleepage(philo);
		thinkage(philo);
	}
	return (NULL);
}
