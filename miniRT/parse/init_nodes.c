/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_nodes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 21:21:56 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/07 14:11:45 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../sphere.h"

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
		return (print_err(INVALID_ERR, args[0]), print_err("\n", NULL), NULL);
}

t_types	*init_sphere(char **args)
{
	double		nb;
	t_sphere	*node;

	if (argv_size(args) != 4)
		return (print_err(QUANTITY_ERR, "Sphere\n"), NULL);
	node = malloc(sizeof(t_sphere));
	if (!node)
		return (NULL);
	node->type = 's';
	node->next = NULL;
	if (init_float_array(args[1], &node->coords, 0, "Sphere\n"))
		return (free(node), NULL);
	nb = ft_atolf(args[2]);
	if (!is_valid_float(args[2], nb) || nb < 0.0)
		return (print_err(NUMBER_ERR, "Sphere\n"), NULL);
	else
		node->radius = nb;
	if (init_int_array(args[3], &node->color, "Sphere\n"))
		return (free(node), NULL);
	return ((t_types *)node);
}

t_types	*init_cylinder(char **args)
{
	double		nb;
	t_cylinder	*node;

	if (argv_size(args) != 6)
		return (print_err(QUANTITY_ERR, "Cylinder\n"), NULL);
	node = malloc(sizeof(t_cylinder));
	if (!node)
		return (NULL);
	node->type = 'y';
	node->next = NULL;
	if (init_float_array(args[1], &node->coords, 0, "Cylinder\n")
		|| init_float_array(args[2], &node->vector, 1, "Cylinder\n")
		|| init_int_array(args[5], &node->color, "Cylinder\n")
		|| check_vector(node->vector, "Cylinder\n"))
		return (free(node), NULL);
	nb = ft_atolf(args[3]);
	if (!is_valid_float(args[3], nb) || nb < 0.0)
		return (print_err(NUMBER_ERR, "Cylinder\n"), NULL);
	else
		node->radius = nb;
	nb = ft_atolf(args[4]);
	if (!is_valid_float(args[4], nb) || nb < 0.0)
		return (print_err(NUMBER_ERR, "Cylinder\n"), NULL);
	else
		node->height = nb;
	return ((t_types *)node);
}

t_types	*init_plane(char **args)
{
	t_plane	*node;

	if (argv_size(args) != 4)
		return (print_err(QUANTITY_ERR, "Plane\n"), NULL);
	node = malloc(sizeof(t_plane));
	if (!node)
		return (NULL);
	node->type = 'p';
	node->next = NULL;
	if (init_float_array(args[1], &node->coords, 0, "Plane\n"))
		return (free(node), NULL);
	if (init_float_array(args[2], &node->vector, 1,"Plane\n")
		|| check_vector(node->vector, "Plane\n"))
		return (free(node), NULL);
	if (init_int_array(args[3], &node->color, "Plane\n"))
		return (free(node), NULL);
	return ((t_types *)node);
}
