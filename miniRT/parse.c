/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 11:29:33 by vloureir          #+#    #+#             */
/*   Updated: 2025/12/14 20:03:19 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"

int	parse_data(t_program *data, int argc, char **argv)
{
	int		count;
	int		fd;
	char	*line;
	t_types	*objects;

	count = 0;
	objects = NULL;
	if (check_args(argc, argv, &fd))
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		if (not_whitespaces(line))
		{
			count++;
			if (parse_line(data, line, &objects))
				return (ft_lstclear(objects), close(fd), free(line), 1);
		}
		free(line);
		line = get_next_line(fd);
	}
	if (count == 0)
		return (close(fd), ft_putstr_fd(EMPTY_ERR, 2), 1);
	data->objects = objects;
	close(fd);
	free(line);
	return (0);
}

int	parse_line(t_program *data, char *str, t_types **objects)
{
	int		len;
	int		flag;
	char	**args;
	t_types	*node;

	if (!str)
		return (1);
	flag = 0;
	len = ft_strlen(str);
	if (str[len - 1] == '\n')
		str[len - 1] = ' ';
	args = ft_split(str, ' '); // Do we need to take care of all whitespaces?
	if (!args)
		return (ft_putstr_fd(MALLOC_ERR, 2), 1);
	if (ft_strcmp(args[0], "A") == 0)
		flag = init_ambient(data, args);
	else if (ft_strcmp(args[0], "C") == 0)
		flag = init_camera(data, args);
	else if (ft_strcmp(args[0], "L") == 0)
		flag = init_light(data, args);
	else
	{
		node = initialize_node(args);
		if (!node)
			return (free_args(args), 1);
		ft_lstadd_back(objects, node);
	}
	free_args(args);
	return (flag);
}

// IN CASE OF INVALID DATA, RETURN 1
int	init_ambient(t_program *data, char **args)
{
	double	num;

	if (argv_size(args) != 3)
		return (ft_putstr_fd(QUANTITY_ERR, 2), 1);
	num = ft_atolf(args[1]);
	if (!is_valid_float(args[1], num) || num < 0.0 || num > 1.0)
		return (ft_putstr_fd(NUMBER_ERR, 2), 1);
	else
		data->ambient.ratio = num;
	if (init_int_array(args[2], &data->ambient.color))
		return (1);
	data->ambient.color.x *= data->ambient.ratio;
	data->ambient.color.y *= data->ambient.ratio;
	data->ambient.color.z *= data->ambient.ratio;
	return (0);
}

int	init_camera(t_program *data, char **args)
{
	int		nb;

	if (argv_size(args) != 4)
		return (ft_putstr_fd(QUANTITY_ERR, 2), 1);
	if (init_float_array(args[1], &data->camera.coords, 0))
		return (1);
	if (init_float_array(args[2], &data->camera.vector, 1))
		return (1);
	nb = ft_atoi(args[3]);
	if (!is_valid_int(args[3]) || nb < 0 || nb > 180)
		return (ft_putstr_fd(NUMBER_ERR, 2), 1);
	else
		data->camera.fov = nb;
	return (0);
}

int	init_light(t_program *data, char **args)
{
	double	nb;

	if (argv_size(args) != 4)
		return (ft_putstr_fd(QUANTITY_ERR, 2), 1);
	if (init_float_array(args[1], &data->light.coords, 0))
		return (1);
	nb = ft_atolf(args[2]);
	if (!is_valid_float(args[2], nb) || nb < 0.0 || nb > 1.0)
		return (ft_putstr_fd(NUMBER_ERR, 2), 1);
	else
		data->light.brigthness = nb;
	if (init_int_array(args[3], &data->light.color))
		return (1);
	return (0);
}

// OLD
// int	init_int_array(char *str, t_uchar *array)
// {
// 	int		i;
// 	int		len;
// 	int		nb;
// 	char	**args;

// 	len = ft_strlen(str);
// 	if (str[len - 1] == ',')
// 		str[len - 1] = ' ';
// 	args = ft_split(str, ',');
// 	if (!args)
// 		return (ft_putstr_fd(MALLOC_ERR, 2), 1);
// 	if (argv_size(args) != 3)
// 		return (free_args(args), ft_putstr_fd(QUANTITY_ERR, 2), 1);
// 	i = -1;
// 	while (args[++i])
// 	{
// 		nb = ft_atoi(args[i]);
// 		if (!is_valid_int(args[i]) || nb < 0 || nb > 255)
// 			return (free_args(args), ft_putstr_fd(NUMBER_ERR, 2), 1);
// 		array[i] = nb;
// 	}
// 	free_args(args);
// 	return (0);
// }

// DONE
int	init_int_array(char *str, t_vec3 *color)
{
	int		i;
	int		len;
	int		nb;
	char	**args;

	len = ft_strlen(str);
	if (str[len - 1] == ',')
		str[len - 1] = ' ';
	args = ft_split(str, ',');
	if (!args)
		return (ft_putstr_fd(MALLOC_ERR, 2), 1);
	if (argv_size(args) != 3)
		return (free_args(args), ft_putstr_fd(QUANTITY_ERR, 2), 1);
	i = -1;
	while (args[++i])
	{
		nb = ft_atoi(args[i]);
		if (!is_valid_int(args[i]) || nb < 0 || nb > 255)
			return (free_args(args), ft_putstr_fd(NUMBER_ERR, 2), 1);
		if (i == 0)
			color->x = (float)nb / 255.0;
		else if (i == 1)
			color->y = (float)nb / 255.0;
		else
			color->z = (float)nb / 255.0;
	}
	free_args(args);
	return (0);
}

int	init_float_array(char *str, t_vec3 *vector, int flag)
{
	int		i;
	int		len;
	double	nb;
	char	**args;

	len = ft_strlen(str);
	if (str[len - 1] == ',')
		str[len - 1] = ' ';
	args = ft_split(str, ',');
	if (!args)
		return (ft_putstr_fd(MALLOC_ERR, 2), 1);
	if (argv_size(args) != 3)
		return (free_args(args), ft_putstr_fd(QUANTITY_ERR, 2), 1);
	i = -1;
	while (args[++i])
	{
		nb = ft_atolf(args[i]);
		if (!is_valid_float(args[i], nb) || (flag && (nb < -1.0 || nb > 1.0)))
			return (free_args(args), ft_putstr_fd(NUMBER_ERR, 2), 1);
		if (i == 0)
			vector->x = nb;
		else if (i == 1)
			vector->y = nb;
		else
			vector->z = nb;
	}
	free_args(args);
	return (0);
}

int	argv_size(char **argv)
{
	int	i;

	i = 0;
	while (argv[i])
		i++;
	return (i);
}

int	is_valid_float(char *str, double nb)
{
	int	i;
	int	sign;
	int	point;

	i = 0;
	sign = 0;
	point = 0;
	(void)nb; // Remove if the check below works
//	The check below doesnt work, why?
	// if (nb < FLT_MIN || nb > FLT_MAX)
	// {
	// 	printf("%lf, %f\n", nb, FLT_MIN);
	// 	return (0);
	// }
	if ((str[i] == '-' || str[i] == '+') && str[i + 1])
		i++;
	while (str[i])
	{
		if (str[i] == '.' && str[i + 1])
			point++;
		else if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	if (point > 1 || sign > 1)
		return (0);
	return (1);
}

int	is_valid_int(char *str)
{
	int	i;

	i = 0;
	if ((str[i] == '-' || str[i] == '+') && str[i + 1])
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	not_whitespaces(char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	while ((str[i] >= '\t' && str[i] <= '\r') || str[i] == ' ')
		i++;
	if (str[i])
		return (1);
	return (0);
}

int	check_extenssion(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	if (i < 4)
		return (1);
	if (ft_strcmp(".rt", &str[i - 3]))
		return (1);
	return (0);
}

int	check_args(int argc, char **argv, int *fd)
{
	if (argc == 1)
	{
		ft_putstr_fd(FILE_ERR, 2);
		return (1);
	}
	if (argc > 2)
	{
		ft_putstr_fd(ARGV_ERR, 2);
		return (1);
	}
	if (check_extenssion(argv[1]))
	{
		ft_putstr_fd(EXTENSION_ERR, 2);
		return (1);
	}
	*fd = open(argv[1], O_RDONLY);
	if (*fd < 0)
	{
		perror("-error");
		return (1);
	}
	return (0);
}

// MAYBE IN HELPER FUNCTIONS
int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

void	free_args(char **argv)
{
	int	i;

	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

// --- LIST UTILS ---
void	ft_lstclear(t_types *lst)
{
	t_types	*ptr;

	ptr = lst;
	while (ptr)
	{
		ptr = ptr->next;
		free(lst);
		lst = ptr;
	}
}

void	ft_lstadd_back(t_types **lst, t_types *node)
{
	t_types	*last;

	if (!lst)
		return ;
	if (!*lst)
		*lst = node;
	else
	{
		last = *lst;
		while (last->next)
			last = last->next;
		last->next = node;
	}
}

// --- INIT NODES ---
t_types	*initialize_node(char **args)
{
	if (!args)
		return (NULL);
	else if (ft_strcmp(args[0], "sp") == 0)
		return (init_sphere(args));
	else if (ft_strcmp(args[0], "pl") == 0)
		return (init_plane(args));
	else if (ft_strcmp(args[0], "cy") == 0)
		return (init_cylinder(args));
	else
		return (ft_putstr_fd(INVALID_ERR, 2), NULL);
}

// IN CASE OF INVALID DATA, RETURN NULL
t_types	*init_sphere(char **args)
{
	double		nb;
	t_sphere	*node;

	if (argv_size(args) != 4)
		return (ft_putstr_fd(QUANTITY_ERR, 2), NULL);
	node = malloc(sizeof(t_sphere));
	if (!node)
		return (NULL);
	node->type = 's';
	node->next = NULL;
	if (init_float_array(args[1], &node->coords, 0))
		return (free(node), NULL);
	nb = ft_atolf(args[2]);
	if (!is_valid_float(args[2], nb))
		return (ft_putstr_fd(NUMBER_ERR, 2), NULL);
	else
		node->radius = nb;
	if (init_int_array(args[3], &node->color))
		return (free(node), NULL);
	return ((t_types *)node);
}

t_types	*init_cylinder(char **args)
{
	double		nb;
	t_cylinder	*node;

	if (argv_size(args) != 6)
		return (ft_putstr_fd(QUANTITY_ERR, 2), NULL);
	node = malloc(sizeof(t_cylinder));
	if (!node)
		return (NULL);
	node->type = 'y';
	node->next = NULL;
	if (init_float_array(args[1], &node->coords, 0)
		|| init_float_array(args[2], &node->vector, 1)
		|| init_int_array(args[5], &node->color))
		return (free(node), NULL);
	nb = ft_atolf(args[3]);
	if (!is_valid_float(args[3], nb))
		return (ft_putstr_fd(NUMBER_ERR, 2), NULL);
	else
		node->radius = nb;
	nb = ft_atolf(args[4]);
	if (!is_valid_float(args[4], nb))
		return (ft_putstr_fd(NUMBER_ERR, 2), NULL);
	else
		node->height = nb;
	return ((t_types *)node);
}

t_types	*init_plane(char **args)
{
	t_plane	*node;

	if (argv_size(args) != 4)
		return (ft_putstr_fd(QUANTITY_ERR, 2), NULL);
	node = malloc(sizeof(t_plane));
	if (!node)
		return (NULL);
	node->type = 'p';
	node->next = NULL;
	if (init_float_array(args[1], &node->coords, 0))
		return (free(node), NULL);
	if (init_float_array(args[2], &node->vector, 1))
		return (free(node), NULL);
	if (init_int_array(args[3], &node->color))
		return (free(node), NULL);
	return ((t_types *)node);
}
