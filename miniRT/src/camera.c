/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 13:34:16 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/16 07:58:31 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minirt.h"

static t_vec3	find_v(t_vec3 camera);

t_vec3	camera_direction(t_program *data, float v, float u)
{
	double		aspect_ratio;
	t_vec3		ray_dir;
	t_cam_math	camera;

	aspect_ratio = (float)WIDTH / (float)HEIGHT;
	camera.height = tan((data->camera.fov * MY_PI / 180.0) / 2.0);
	camera.width = aspect_ratio * camera.height;
	camera.vertical = find_v(data->camera.vector);
	camera.w = data->camera.vector;
	camera.v = normalize_vector(vec_cross(camera.w, camera.vertical));
	camera.u = normalize_vector(vec_cross(camera.v, camera.w));
	ray_dir = vec_add(vec_add(vec_scale(camera.v, v * camera.height),
				vec_scale(camera.u, u * camera.width)), camera.w);
	return (ray_dir);
}

static t_vec3	find_v(t_vec3 camera)
{
	if (camera.x == 0.0 && camera.z == 0.0
		&& (camera.y == 1.0 || camera.y == -1.0))
		return ((t_vec3){1.0, 0.0, 0.0});
	return ((t_vec3){0.0, 1.0, 0.0});
}
