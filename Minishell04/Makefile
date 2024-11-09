# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jkalinow <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/08 12:01:57 by jkalinow          #+#    #+#              #
#    Updated: 2024/09/08 12:02:51 by jkalinow         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 				= minishell

CC				= cc
CFLAGS				= -g -Wall -Wextra -Werror
LDFLAGS 			= -lreadline
RM				= rm -f

SRC				= $(wildcard srcs/*.c)
OBJ				= $(SRC:.c=.o)

all: $(NAME)

$(NAME):			$(OBJ)
				$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
				$(RM) $(OBJ)

fclean:				clean
				$(RM) $(NAME)

re:				fclean all

.PHONY:				all clean fclean re
