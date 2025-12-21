/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_data.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 21:24:16 by vloureir          #+#    #+#             */
/*   Updated: 2025/12/21 11:43:45 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../sphere.h"

int	init_ambient(t_program *data, char **args)
{
	double	num;

	if (argv_size(args) != 3)
		return (print_err(QUANTITY_ERR, "Ambient\n"), 1);
	num = ft_atolf(args[1]);
	if (!is_valid_float(args[1], num) || num < 0.0 || num > 1.0)
		return (print_err(NUMBER_ERR, "Ambient\n"), 1);
	else
		data->ambient.ratio = num;
	if (init_int_array(args[2], &data->ambient.color, "Ambient\n"))
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
		return (print_err(QUANTITY_ERR, "Camera\n"), 1);
	if (init_float_array(args[1], &data->camera.coords, 0, "Camera\n"))
		return (1);
	if (init_float_array(args[2], &data->camera.vector, 1, "Camera\n")
		|| check_vector(data->camera.vector, "Camera\n"))
		return (1);
	nb = ft_atoi(args[3]);
	if (!is_valid_int(args[3]) || nb < 0 || nb > 180)
		return (print_err(NUMBER_ERR, "Camera\n"), 1);
	else
		data->camera.fov = nb;
	return (0);
}

int	init_light(t_program *data, char **args)
{
	double	nb;

	if (argv_size(args) != 4)
		return (print_err(QUANTITY_ERR, "Light\n"), 1);
	if (init_float_array(args[1], &data->light.coords, 0, "Light\n"))
		return (1);
	nb = ft_atolf(args[2]);
	if (!is_valid_float(args[2], nb) || nb < 0.0 || nb > 1.0)
		return (print_err(NUMBER_ERR, "Light\n"), 1);
	else
		data->light.brigthness = nb;
	if (init_int_array(args[3], &data->light.color, "Light\n"))
		return (1);
	return (0);
}

int	init_int_array(char *str, t_vec3 *color, char *type)
{
	int		i;
	int		len;
	int		nb;
	char	**args;

	correct_string(str, ',');
	args = ft_split(str, ',');
	if (!args)
		return (ft_putstr_fd(MALLOC_ERR, 2), 1);
	if (argv_size(args) != 3)
		return (free_args(args), print_err(QUANTITY_ERR, type), 1);
	i = -1;
	while (args[++i])
	{
		nb = ft_atoi(args[i]);
		if (!is_valid_int(args[i]) || nb < 0 || nb > 255)
			return (free_args(args), print_err(NUMBER_ERR, type), 1);
		if (i == 0)
			color->x = (float)nb / 255.0;
		else if (i == 1)
			color->y = (float)nb / 255.0;
		else
			color->z = (float)nb / 255.0;
	}
	return (free_args(args), 0);
}

int	init_float_array(char *str, t_vec3 *vector, int flag, char *type)
{
	int		i;
	int		len;
	double	nb;
	char	**args;

	correct_string(str, ',');
	args = ft_split(str, ',');
	if (!args)
		return (ft_putstr_fd(MALLOC_ERR, 2), 1);
	if (argv_size(args) != 3)
		return (free_args(args), print_err(QUANTITY_ERR, type), 1);
	i = -1;
	while (args[++i])
	{
		nb = ft_atolf(args[i]);
		if (!is_valid_float(args[i], nb) || (flag && (nb < -1.0 || nb > 1.0)))
			return (free_args(args), print_err(NUMBER_ERR, type), 1);
		if (i == 0)
			vector->x = nb;
		else if (i == 1)
			vector->y = nb;
		else
			vector->z = nb;
	}
	return (free_args(args), 0);
}
