NAME := miniRT

SRC := $(addprefix src/, camera.c error_and_exit.c main.c mlx_utils.c render.c render_utils.c \
		vec_utils1.c vec_utils2.c sphere.c plane.c cylinder.c cylinder_helper.c)

PARSE := $(addprefix parse/, helper_functions.c init_data.c init_nodes.c lst_utils.c parse.c)

LIBFT := $(addprefix libft_utils/, ft_split.c ft_putstr_fd.c ft_strlcpy.c ft_atoi.c ft_atolf.c)

GNL := $(addprefix gnl/, get_next_line.c get_next_line_utils.c)

OBJ := $(SRC:.c=.o) $(PARSE:.c=.o) $(LIBFT:.c=.o) $(GNL:.c=.o)

CC := cc

CFLAGS := -Wall -Werror -Wextra

LFLAGS := -Lminilibx-linux -lmlx_Linux -lX11 -lXext -lm

RM := rm -f

REPO := https://github.com/42paris/minilibx-linux.git

MLX := ./minilibx-linux/

HEADER := minirt.h

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	$(MAKE) -C $(MLX)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LFLAGS) -I.

mlx:
	git clone $(REPO)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re mlx