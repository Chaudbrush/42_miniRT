/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 11:29:33 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/15 12:22:12 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minirt.h"

int	parse_data(t_program *data, int argc, char **argv, int count)
{
	int		fd;
	char	*line;
	t_types	*objects;

	objects = NULL;
	if (check_args(argc, argv, &fd))
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		if (not_whitespaces(line))
		{
			count++;
			if (parse_line(data, line, &objects, 0))
				return (ft_lstclear(objects), close(fd), free(line), 1);
		}
		free(line);
		line = get_next_line(fd);
	}
	if (count == 0)
		return (close(fd), ft_putstr_fd(EMPTY_ERR, 2), 1);
	data->objects = objects;
	index_list(data->objects);
	return (close(fd), free(line), 0);
}

int	parse_line(t_program *data, char *str, t_types **objects, int flag)
{
	int		len;
	char	**args;
	t_types	*node;

	correct_string(str, '\n');
	args = ft_split(str, ' ');
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
	return (free_args(args), flag);
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

int	is_valid_float(char *str, double nb)
{
	int	i;
	int	sign;
	int	point;

	i = 0;
	sign = 0;
	point = 0;
	if (nb < -FLT_MAX || nb > FLT_MAX)
		return (0);
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
