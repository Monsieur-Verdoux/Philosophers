/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 20:15:20 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/06 21:34:11 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H

# define PHILOSOPHERS_H
# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include <sys/time.h>
# include <stdbool.h>
# define DEAD	0
# define ALIVE	1
# define EAT	1
# define SLEEP	2
# define THINK	3

typedef struct s_info
{
	size_t			number_of_philosophers;
	size_t			time_to_die;
	size_t			time_to_eat;
	size_t			time_to_sleep;
	size_t			number_of_times_each_philosopher_must_eat;
	pthread_mutex_t	**forks;
	pthread_mutex_t	*print;
	void			**philos;
	size_t			init;
	bool			death;
	pthread_mutex_t	*death_mutex;
}	t_info;

typedef struct s_philos
{
	int			id;
	pthread_t		thread;
	int				status;
	t_info			*info;
	int				left;
	int				right;
	bool			alive;
	size_t			meal_count;
	size_t			last_meal;
	size_t			since_last_meal;
	pthread_mutex_t	*alive_mutex;
	pthread_mutex_t	*eat_mutex;
	size_t			init;
}	t_philos;

#endif 