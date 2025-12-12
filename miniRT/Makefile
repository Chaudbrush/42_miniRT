NAME := RayTracing

SRC := sphere.c parse.c gnl/get_next_line_utils.c gnl/get_next_line.c \
		libft/ft_split.c libft/ft_putstr_fd.c libft/ft_strlcpy.c libft/ft_atoi.c libft/ft_atolf.c

OBJ := $(SRC:.c=.o)

CC := cc

CFLAGS := 

LFLAGS := -Lminilibx-linux -lmlx_Linux -lX11 -lXext -lm

RM := rm -f

MLX := ./minilibx-linux/

HEADER := sphere.h

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	$(MAKE) -C $(MLX)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LFLAGS) -I.

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re