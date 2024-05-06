# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/02 20:17:28 by akovalev          #+#    #+#              #
#    Updated: 2024/05/06 15:23:08 by akovalev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= philosophers

CFLAGS	:= -pthread -fsanitize=thread #-Wextra -Wall -Werror -Wunreachable-code -Ofast 
LDFLAGS := -pthread -fsanitize=thread

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