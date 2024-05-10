/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:00:42 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 14:29:47 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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
