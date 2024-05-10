/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 20:17:16 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 14:44:51 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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
		if (ft_atoi_long(argv[i]) > 2147483647 || ft_atoi_long(argv[i]) == 0)
		{
			ft_putstr_fd("Please use positive integers as arguments\n", 2);
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

int	main(int argc, char **argv)
{
	t_info		info;
	t_philos	**philos;

	if (!input_validation(argv, argc) || \
		!initialize(argv, argc, &info) || !forge_forks(&info))
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
