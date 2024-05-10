# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/02 20:17:28 by akovalev          #+#    #+#              #
#    Updated: 2024/05/10 14:26:48 by akovalev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= philo
CFLAGS	:= -pthread -Wextra -Wall -Werror -Wunreachable-code -Ofast 
LDFLAGS := -pthread
SRCS	:= main.c cleanup.c tools.c tools1.c philosophize.c death_and_gluttony.c spawning_pit.c
OBJS	:= ${SRCS:.c=.o} 

all: $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) -o $@ -c $< 

$(NAME): $(OBJS) 
	@$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -rf $(NAME)

re: clean all

.PHONY: all, clean, fclean, re 