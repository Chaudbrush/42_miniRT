/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 11:29:33 by vloureir          #+#    #+#             */
/*   Updated: 2025/11/29 13:12:20 by vloureir         ###   ########.fr       */
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
	{
		printf("-error: Empty file\n");
		return (1);
	}
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
	args =  ft_split(str, ' '); // Do we need to take care of all whitespaces?
	if (ft_strcmp(args[0], "A") == 0)
		flag = init_ambient(data, args);
	else if (ft_strcmp(args[0], "C") == 0)
		flag = init_camera(data, args);
	else if (ft_strcmp(args[0], "L") == 0)
		flag = init_light(data, args);
	else
	{
//		printf("a\n");
		node = initialize_node(args);
		if (!node)
			return (free_args(args), 1);
		ft_lstadd_back(objects, node);
	}
	free_args(args);
	return (flag);
}

t_types	*initialize_node(char **args)
{
//	printf("b\n");
	if (!args)
		return (NULL);
	else if (ft_strcmp(args[0], "sp") == 0)
		return (init_sphere(args));
	else if (ft_strcmp(args[0], "pl") == 0)
		return (init_plane(args));
	else if (ft_strcmp(args[0], "cy") == 0)
		return (init_cylinder(args));
	else
		return (NULL);
}

void	ft_lstadd_back(t_types **lst, t_types *node)
{
	t_types	*last;


	if (!lst)
		return ;

	// print_list(*lst);
	// printf("node: %c\n", node->type);	
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

// IN CASE OF INVALID DATA, RETURN 1
int	init_ambient(t_program *data, char **args)
{
	int		i;
	int		nb;
	double	num;
	char	**nums;

	if (argv_size(args) != 3)
		return (ft_putstr_fd("-error: Missing or Extra arguments\n", 2), 1);
	i = -1;
	num = ft_atolf(args[1]);
	if (!is_valid_float(args[1], num) || num < 0.0 || num > 1.0)
		return (ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
	else
		data->ambient_light.ratio = num;
	nums = ft_split(args[2], ',');
	if (!nums)
		return (1);
	while (nums[++i])
	{
		nb = ft_atoi(nums[i]);
		if (!is_valid_int(nums[i]) || num < 0 || num > 255)
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
		data->ambient_light.color[i] = nb;
	}
	free_args(nums);
	return (0);
}

int	init_camera(t_program *data, char **args)
{
	int		i;
	int		nb;
	double	num;
	char	**nums;

	printf("camera1\n");
	if (argv_size(args) != 4)
		return (ft_putstr_fd("-error: Missing or Extra arguments\n", 2), 1);
	i = -1;
	nums = ft_split(args[1], ',');
	if (!nums)
		return (1);
	while (nums[++i])
	{
		nb = ft_atolf(nums[i]);
		if (!is_valid_float(nums[i], nb))
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
		data->camera.coords[i] = nb;
	}
	free_args(nums);
	printf("camera2\n");
	i = -1;
	nums = ft_split(args[2], ',');
	if (!nums)
		return (1);
	while (nums[++i])
	{
		nb = ft_atolf(nums[i]);
		if (!is_valid_float(nums[i], nb) || nb < -1.0 || nb > 1.0)
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
		data->camera.norm_vector[i] = nb;
	}
	free_args(nums);
	num = ft_atoi(args[3]);
	if (!is_valid_int(args[3]) || num < 0 || num > 180)
		return (ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
	else
		data->camera.fov = num;
	return (0);



	
	/*
	data->camera.coords[0] = 0;
	data->camera.coords[1] = 0;
	data->camera.coords[2] = 0;
	data->camera.norm_vector[0] = 0;
	data->camera.norm_vector[1] = 0;
	data->camera.norm_vector[2] = 0;
	data->camera.fov = 0; // [0, 180]
	*/
}

int	init_light(t_program *data, char **args)
{
	int		i;
	int		nb;
	double	num;
	char	**nums;

	printf("light\n");
	if (argv_size(args) != 4)
		return (ft_putstr_fd("-error: Missing or Extra arguments\n", 2), 1);
	i = -1;
	nums = ft_split(args[1], ',');
	if (!nums)
		return (1);
	while (nums[++i])
	{
		nb = ft_atolf(nums[i]);
		if (!is_valid_float(nums[i], nb))
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
		data->light.coords[i] = nb;
	}
	free_args(nums);
	i = -1;
	nums = ft_split(args[3], ',');
	if (!nums)
		return (1);
	while (nums[++i])
	{
		nb = ft_atoi(nums[i]);
		if (!is_valid_int(nums[i]) || nb < 0 || nb > 255)
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
		data->light.color[i] = nb;
	}
	free_args(nums);
	num = ft_atolf(args[2]);
	if (!is_valid_float(args[2], num) || num < 0.0 || num > 1.0)
		return (ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
	else
		data->light.brigthness = num;
	return (0);
	/*
	data->light.coords[0] = 0;
	data->light.coords[1] = 0;
	data->light.coords[2] = 0;
	data->light.brightness = 0;
	data->light.color[0] = 0;
	data->light.color[1] = 0;
	data->light.color[2] = 0;
	*/
}


// // IN CASE OF INVALID DATA, RETURN NULL
t_types	*init_sphere(char **args)
{
	int			i;
	int			nb;
	double		num;
	t_sphere	*node;
	char		**nums;

	printf("sphere\n");
	if (argv_size(args) != 4)
		return (ft_putstr_fd("-error: Missing or Extra arguments\n", 2), NULL);
	node = malloc(sizeof(t_sphere));
	if (!node)
		return (NULL);
	node->type = 's';
	node->next = NULL;

	// Coords
	i = -1;
	nums = ft_split(args[1], ',');
	if (!nums)
		return (NULL);
	while (nums[++i])
	{
		nb = ft_atolf(nums[i]);
		if (!is_valid_float(nums[i], nb))
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
		node->coords[i] = nb;
	}
	free_args(nums);

	
	// Radius
	num = ft_atolf(args[2]);
	if (!is_valid_float(args[2], num))
		return (ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
	else
		node->radius = num;


	// rgb
	i = -1;
	nums = ft_split(args[3], ',');
	if (!nums)
		return (NULL);
	while (nums[++i])
	{
		nb = ft_atoi(nums[i]);
		if (!is_valid_int(nums[i]) || nb < 0 || nb > 255)
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
		node->rgb[i] = nb;
	}
	free_args(nums);
	
	return ((t_types *)node);
	/*
		coords
		diamenter
		color		[0,255] xyz
	*/
}

t_types	*init_cylinder(char **args)
{
	int			i;
	int			nb;
	double		num;
	t_cylinder	*node;
	char		**nums;

	if (argv_size(args) != 6)
		return (ft_putstr_fd("-error: Missing or Extra arguments\n", 2), NULL);
	node = malloc(sizeof(t_cylinder));
	if (!node)
		return (NULL);
	node->type = 'y';
	node->next = NULL;

	// Coords
	i = -1;
	nums = ft_split(args[1], ',');
	if (!nums)
		return (NULL);
	while (nums[++i])
	{
		nb = ft_atolf(nums[i]);
		if (!is_valid_float(nums[i], nb))
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
		node->coords[i] = nb;
	}
	free_args(nums);

	// Vector
	i = -1;
	nums = ft_split(args[2], ',');
	if (!nums)
		return (NULL);
	while (nums[++i])
	{
		nb = ft_atolf(nums[i]);
		if (!is_valid_float(nums[i], nb) || nb < 0.0 || nb > 1.0)
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
		node->coords[i] = nb;
	}
	free_args(nums);

	// Radius
	num = ft_atolf(args[3]);
	if (!is_valid_float(args[3], num))
		return (ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
	else
		node->radius = num;

	// Height
	num = ft_atolf(args[4]);
	if (!is_valid_float(args[4], num))
		return (ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
	else
		node->radius = num;

	// rgb
	i = -1;
	nums = ft_split(args[5], ',');
	if (!nums)
		return (NULL);
	while (nums[++i])
	{
		nb = ft_atoi(nums[i]);
		if (!is_valid_int(nums[i]) || nb < 0 || nb > 255)
			return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
		node->rgb[i] = nb;
	}
	free_args(nums);
	
	return ((t_types *)node);





	/*
		coords
		norm_vector [-1,1] xyz
		diameter
		height
		color		[0,255] xyz
	*/
}



t_types	*init_plane(char **args)
{
//	t_plane	*node;

	(void)args;
	return (NULL);
	// node = malloc(sizeof(t_plane));
	// if (!node)
	// 	return (NULL);
	// node->type = 'p';
	// node->next = NULL;	


	// int			i;
	// int			nb;
	// double		num;
	// t_sphere	*node;
	// char		**nums;

	// (void)args;
	// node = malloc(sizeof(t_sphere));
	// if (!node)
	// 	return (NULL);
	// node->type = 's';
	// node->next = NULL;

	// // Coords
	// i = -1;
	// nums = ft_split(args[1], ',');
	// if (!nums)
	// 	return (1);
	// while (nums[++i])
	// {
	// 	nb = ft_atolf(nums[i]);
	// 	if (!is_valid_float(nums[i], nb))
	// 		return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
	// 	node->coords[i] = nb;
	// }
	// free_args(nums);

	
	// // Radius
	// num = ft_atolf(args[2]);
	// if (!is_valid_float(args[2], num) || num < 0.0 || num > 1.0)
	// 	return (ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), NULL);
	// else
	// 	node->radius = num;


	// // rgb
	// i = -1;
	// nums = ft_split(args[2], ',');
	// if (!nums)
	// 	return (1);
	// while (nums[++i])
	// {
	// 	nb = ft_atoi(nums[i]);
	// 	if (!is_valid_int(nums[i]) || num < 0 || num > 255)
	// 		return (free_args(nums), ft_putstr_fd("-error: Invalid Numeric arguments\n", 2), 1);
	// 	node->rgb[i] = nb;
	// }
	// free_args(nums);
	
	// return ((t_types *)node);
	
	/*
		coords
		norm_vector	[-1,1] xyz
		color		[0,255] xyz
	*/
}



// DONE

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

	i = -1;
	sign = 0;
	point = 0;
	(void)nb;
	// The check below doesnt work, why?
	// if (nb < FLT_MIN || nb > FLT_MAX)
	// {
	// 	printf("%lf, %f\n", nb, FLT_MIN);
	// 	return (0);
	// }
	while (str[++i])
	{
		if (str[i] == '.')
			point++;
		else if (str[i] == '-' || str[i] == '+')
			sign++;
		else if (str[i] < '0' || str[i] > '9')
			return (0);
	}
	if (point > 1 || sign > 1)
		return (0);
	return (1);
}

int	is_valid_int(char *str)
{
	int	i;
	int	sign;

	i = -1;
	sign = 0;
	while (str[++i])
	{
		if (str[i] == '-' || str[i] == '+')
			sign++;
		else if (str[i] < '0' || str[i] > '9')
			return (0);
	}
	if (sign > 1)
		return (0);
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
		ft_putstr_fd("-error: Missing file argument\n",2);
		return (1);
	}
	if (argc > 2)
	{
		ft_putstr_fd("-error: Too many arguments\n",2);
		return (1);
	}	
	if (check_extenssion(argv[1]))
	{
		ft_putstr_fd("-error: Wrong extension\n",2);
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