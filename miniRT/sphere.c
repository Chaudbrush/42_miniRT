/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:25:11 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/15 16:33:13 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int raytrace_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, t_program *data)
{
	t_vec3	oc;
	t_vec3 center = sphere->coords;
	t_vec3	ray_origin = data->camera.coords;

    // Quadratic coefficients
//    t_vec3 oc = { ray_origin.x - center.x, ray_origin.y - center.y, ray_origin.z - center.z };
	oc = vec_sub(ray_origin, sphere->coords);
    float a = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    float b = 2.0f * (oc.x * dir.x + oc.y * dir.y + oc.z * dir.z);
    float c = oc.x*oc.x + oc.y*oc.y + oc.z*oc.z - sphere->radius * sphere->radius;

    float disc = b * b - 4 * a * c;
    if (disc < 0.0f) return -1; // no hit

    float sqrtD = sqrtf(disc);
    float t0 = (-b - sqrtD) / (2 * a);
    float t1 = (-b + sqrtD) / (2 * a);

    // Pick nearest positive intersection
    float t = (t0 > 0) ? t0 : t1;
    if (t < 0) return 0xFFFF0000;
	data->hitpoint = t;
    // Hitpoint
    t_vec3 hit = { ray_origin.x + dir.x*t, ray_origin.y + dir.y*t, ray_origin.z + dir.z*t };
	// hit = vec_scale(hit, 0.99991);

    // Normal at hitpoint
    t_vec3 normal = { hit.x - center.x, hit.y - center.y, hit.z - center.z };
	normal = normalize_vector(normal);

	t_vec3	light_dir = normalize_vector(vec_sub(hit, light));
	if (check_shadow(data, hit, vec_scale(light_dir, -1.0), sphere->index))
		return (to_rgb(vec_mult(data->ambient.color, sphere->color)));
	else
		return (get_color(data, sphere->color, normal, light_dir));
}

int	shadow_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, t_vec3 ray_origin)
{
//	t_vec3	center;
	t_vec3	calc;
	t_vec3	oc;
	float	light_dist;

	// center = sphere->coords;
	// oc = (t_vec3){ray_origin.x - center.x, ray_origin.y - center.y, ray_origin.z - center.z};
	
	oc = vec_sub(ray_origin, sphere->coords);
	calc.x = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z; // a
	calc.y = 2.0f * (oc.x * dir.x + oc.y * dir.y + oc.z * dir.z); // b
	calc.z = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - sphere->radius * sphere->radius; // c

	float disc = calc.y * calc.y - 4 * calc.x * calc.z;
	if (disc < 0.0f)
		return (0);
	float first = ray_origin.x * ray_origin.x + light.x * light.x + ray_origin.y * ray_origin.y + light.y * light.y + ray_origin.z * ray_origin.z + light.z * light.z;

	light_dist = sqrtf(first - 2 * (ray_origin.x * light.x) - 2 * (ray_origin.y * light.y) - 2 * (ray_origin.z * light.z));

	float sqrtD = sqrtf(disc);
	float t0 = (-calc.y - sqrtD) / (2 * calc.x);
	float t1 = (-calc.y + sqrtD) / (2 * calc.x);
	float t = (t0 > 0) ? t0 : t1;
	if (t > 0 && t < light_dist)
		return (1);
	return (0);
}
