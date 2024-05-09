# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/02 20:17:28 by akovalev          #+#    #+#              #
#    Updated: 2024/05/09 18:03:15 by akovalev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= philo

CFLAGS	:= -pthread -Wextra -Wall -Werror -Wunreachable-code -Ofast 
LDFLAGS := -pthread

SRCS	:= main.c 
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