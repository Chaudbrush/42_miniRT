/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:27:53 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/15 16:25:46 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

// int	closest_vec(int *colors, t_vec3 **vecs, int total, t_vec3 camera)
// {
// 	int		i;
// 	int		smallest_idx;
// 	float	tdist;
// 	float	dist;
// 	t_vec3	*vec;

// 	i = -1;
// 	dist = 99999;
// 	smallest_idx = 0;
// 	while (++i < total)
// 	{
// 		if (colors[i] == -1)
// 		{
// 			i++;
// 			continue ;
// 		}
// 		vec = vecs[i];
// 		tdist = sqrt((camera.x - vec->x) * (camera.x - vec->x)
// 				+ (camera.y - vec->y) * (camera.y - vec->y)
// 				+ (camera.z - vec->z) * (camera.z - vec->z));
// 		if (tdist < dist)
// 		{
// 			dist = tdist;
// 			smallest_idx = i;
// 		}
// 	}
// 	if (colors[smallest_idx] == -1)
// 		return (0);
// 	return (colors[smallest_idx]);
// }

int	closest_vec(int *colors, t_vec3 **vecs, int total, t_vec3 camera)
{
	int		i;
	int		smallest_idx;
	float	tdist;
	float	dist;
	t_vec3	*vec;

	i = -1;
	dist = 99999;
	smallest_idx = 0;
	while (++i < total)
	{
		if (colors[i] != -1)
		{
			vec = vecs[i];
			tdist = sqrt((camera.x - vec->x) * (camera.x - vec->x)
					+ (camera.y - vec->y) * (camera.y - vec->y)
					+ (camera.z - vec->z) * (camera.z - vec->z));
			if (tdist < dist)
			{
				dist = tdist;
				smallest_idx = i;
			}
		}
	}
	if (colors[smallest_idx] == -1)
		return (0);
	return (colors[smallest_idx]);
}

int	get_color(t_program *data, t_vec3 color, t_vec3 normal, t_vec3 light)
{
	float	diffuse_dot;
	t_vec3	ambient;
	t_vec3	diffuse;

	ambient = vec_mult(data->ambient.color, color);
	diffuse_dot = -dot_product(normal, light);
	diffuse_dot *= data->light.brigthness;
	if (diffuse_dot < 0)
		diffuse_dot = 0;
	diffuse = vec_scale(color, diffuse_dot);
	return (to_rgb(vec_add(ambient, diffuse)));
}

int	check_shadow(t_program *data, t_vec3 ray_origin, t_vec3 ray_dir, int i)
{
	int		flag;
	t_types	*obj;

	flag = 0;
	obj = data->objects;
	while (obj)
	{
		if (obj->type == 's' && ((t_sphere *)obj)->index != i)
			flag = shadow_sphere(ray_dir, (t_sphere *)obj,
					data->light.coords, ray_origin);
		else if (obj->type == 'y' && ((t_sphere *)obj)->index != i)
			flag = shadow_cylinder(ray_dir, (t_cylinder *)obj,
					data->light.coords, ray_origin);
		else if (obj->type == 'p' && ((t_sphere *)obj)->index != i)
			flag = shadow_plane(ray_dir, (t_plane *)obj,
					data->light.coords, ray_origin);
		if (flag)
			return (flag);
		obj = obj->next;
	}
	return (flag);
}
