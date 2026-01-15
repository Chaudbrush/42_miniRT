/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:14:03 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/15 15:30:30 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	raytrace_plane(t_vec3 dir, t_plane *plane, t_vec3 light, t_program *data)
{
	float	t;
	float	denominator;
	t_vec3	sub;
	t_vec3	hit;
	t_vec3	light_dir;

	sub = vec_sub(plane->coords, data->camera.coords);
	denominator = dot_product(plane->vector, dir);
	if (denominator > 1e-6)
	{
		t = (dot_product(sub, plane->vector)) / denominator;
		if (t < 0)
			return (-1);
		data->hitpoint = t;
		hit = (t_vec3){data->camera.coords.x + dir.x * t,
			data->camera.coords.y + dir.y * t,
			data->camera.coords.z + dir.z * t};
		light_dir = normalize_vector(vec_sub(hit, light));
		if (check_shadow(data, hit, vec_scale(light_dir, -1.0), plane->index))
			return (to_rgb(vec_mult(data->ambient.color, plane->color)));
		else
			return (get_color(data, plane->color,
					vec_scale(plane->vector, -1.0), light_dir));
	}
	return (-1);
}

int	shadow_plane(t_vec3 dir, t_plane *plane, t_vec3 light, t_vec3 ray_origin)
{
	float	t;
	float	first;
	float	denominator;
	float	light_dist;
	t_vec3	sub;

	sub = vec_sub(plane->coords, ray_origin);
	denominator = dot_product(plane->vector, dir);
	first = ray_origin.x * ray_origin.x + light.x * light.x
		+ ray_origin.y * ray_origin.y + light.y * light.y
		+ ray_origin.z * ray_origin.z + light.z * light.z;
	light_dist = sqrtf(first - 2 * (ray_origin.x * light.x)
			- 2 * (ray_origin.y * light.y)
			- 2 * (ray_origin.z * light.z));
	if (denominator > 1e-6)
	{
		t = (dot_product(sub, plane->vector)) / denominator;
		if (t > 0 && t < light_dist)
			return (1);
		return (0);
	}
	return (0);
}
