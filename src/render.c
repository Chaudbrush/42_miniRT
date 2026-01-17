/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 13:24:22 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/16 07:58:50 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minirt.h"

int	render(t_program *data)
{
	int		color;
	float	x;
	float	y;
	double	x_screen;
	double	y_screen;

	y = -1.0;
	while (++y < HEIGHT)
	{
		x = -1.0;
		while (++x < WIDTH)
		{
			x_screen = x * 2 / WIDTH - 1;
			y_screen = y * 2 / HEIGHT - 1;
			color = rt(data, camera_direction(data, x_screen, y_screen),
					0, data->light.coords);
			my_pixel_put(&data->img_data, x, y, color);
		}
	}
	mlx_put_image_to_window(data->mlx, data->win, data->img_data.img, 0, 0);
	return (0);
}

int	rt(t_program *data, t_vec3 ray_dir, int color, t_vec3 light)
{
	t_types	*obj;
	int		final_color;
	float	nearest_t;

	nearest_t = 1e9f;
	obj = data->objects;
	final_color = to_rgb(data->ambient.color);
	while (obj)
	{
		data->hitpoint = -1;
		if (obj->type == 's')
			color = raytrace_sphere(ray_dir, (t_sphere *)obj, light, data);
		else if (obj->type == 'y')
			color = raytrace_cylinder(ray_dir, (t_cylinder *)obj, light, data);
		else if (obj->type == 'p')
			color = raytrace_plane(ray_dir, (t_plane *)obj, light, data);
		if (data->hitpoint > 0 && data->hitpoint < nearest_t && color != -1)
		{
			nearest_t = data->hitpoint;
			final_color = color;
		}
		obj = obj->next;
	}
	return (final_color);
}

int	to_rgb(t_vec3 c)
{
	int	r;
	int	g;
	int	b;

	r = (int)(fminf(fmaxf(c.x, 0.0f), 1.0f) * 255.0f);
	g = (int)(fminf(fmaxf(c.y, 0.0f), 1.0f) * 255.0f);
	b = (int)(fminf(fmaxf(c.z, 0.0f), 1.0f) * 255.0f);
	return ((r << 16) | (g << 8) | b);
}
