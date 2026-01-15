/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zali <zali@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:25:11 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/15 22:26:42 by zali             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static float	quadratic_helper(t_vec3 dir, t_vec3 oc, float radius)
{
	float	a;
	float	b;
	float	c;
	float	t;
	float	sqrt_h;

	a = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
	b = 2.0f * (oc.x * dir.x + oc.y * dir.y + oc.z * dir.z);
	c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - radius * radius;
	sqrt_h = b * b - 4 * a * c;
	if (sqrt_h < 0.0f)
		return (-1);
	sqrt_h = sqrtf(sqrt_h);
	if ((-b - sqrt_h) / (2 * a) > 0.0f)
		t = (-b - sqrt_h) / (2 * a);
	else
		t = (-b + sqrt_h) / (2 * a);
	if (t <= 0.0f)
		return (-1);
	return (t);
}

int	raytrace_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, t_program *data)
{
	t_vec3	ray_origin;
	t_vec3	hit;
	t_vec3	light_dir;
	int		t;

	ray_origin = data->camera.coords;
	t = quadratic_helper(dir, vec_sub(ray_origin, sphere->coords),
			sphere->radius);
	if (t < 0)
		return (0xFFFF0000);
	data->hitpoint = t;
	hit = vec_add(ray_origin, vec_scale(dir, t));
	light_dir = normalize_vector(vec_sub(hit, light));
	if (check_shadow(data, hit, vec_scale(light_dir, -1.0), sphere->index))
		return (to_rgb(vec_mult(data->ambient.color, sphere->color)));
	else
		return (get_color(data, sphere->color, normalize_vector(vec_sub(hit,
						sphere->coords)), light_dir));
}

int	shadow_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, t_vec3 ray_origin)
{
	float	t;
	float	light_dist;
	t_vec3	d;

	d = vec_sub(ray_origin, light);
	light_dist = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
	t = quadratic_helper(dir, vec_sub(ray_origin, sphere->coords),
			sphere->radius);
	if (t > 0 && t < light_dist)
		return (1);
	return (0);
}
