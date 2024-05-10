/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 20:15:20 by akovalev          #+#    #+#             */
/*   Updated: 2024/05/10 15:13:17 by akovalev         ###   ########.fr       */
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
# include <stdio.h>
# include <string.h>

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
	bool			death_or_full;
	pthread_mutex_t	*print_mutex;
	pthread_mutex_t	*start_mutex;
}	t_info;

typedef struct s_philos
{
	int				id;
	pthread_t		thread;
	t_info			*info;
	int				right;
	size_t			meal_count;
	size_t			last_meal;
	size_t			since_last_meal;
	pthread_mutex_t	*eat_mutex;
	size_t			init;
	size_t			number_of_philosophers;
	size_t			number_of_times_each_philosopher_must_eat;
	size_t			time_to_die;
	size_t			time_to_eat;
	size_t			time_to_sleep;
}	t_philos;

//cleanup.c
void	demutexize(t_info *info);
void	dephilosize(t_philos **philos);
void	cleanup(t_info *info, t_philos **philos);

//tools.c
void	ft_putstr_fd(char *s, int fd);
void	*ft_calloc(size_t count, size_t size);
bool	check_malloc_fail(void *p);
int		ft_isdigit(int c);

//tools1.c
size_t	get_current_time(void);
int		ft_newsleep(size_t milliseconds);
long	ft_atoi_long(const char *str);

//philosophize.c
void	*philosophize(void *ptr);

//death_and_gluttony.c
void	lonely_death(t_philos *p);
bool	potential_perishment(t_philos *p);
void	overseer(t_philos **philos);

//spawning_pit.c
int		cradle_of_philosophy(t_info *info, t_philos **philos);
int		forge_forks(t_info *info);

#endif 