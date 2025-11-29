/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:25:11 by vloureir          #+#    #+#             */
/*   Updated: 2025/11/29 14:53:47 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"

int	main(int argc, char **argv)
{
	t_program	data;
	
	if (parse_data(&data, argc, argv))
		exit (1); // Check errors
	init_mlx(&data);
	init_data(&data); // Gonna be where we parse and init the data into the struct
	mlx_loop_hook(data.mlx, render, &data);
	mlx_hook(data.win, DestroyNotify, NoEventMask, clean_exit, &data);
	mlx_hook(data.win, KeyPress, KeyPressMask, key_handler, &data);
	mlx_loop(data.mlx);
}

int	render(t_program *data)
{
	float	x;
	float	y;
	double	scale_x;
	double	scale_y;

	y = -1.0;
	while (++y < HEIGHT)
	{
		x = -1.0;
		while (++x < WIDTH)
		{
			scale_x = (2 * (x /	WIDTH)) - 1;
			scale_y = (2 * (y /	HEIGHT)) - 1;
			
			data->circle.rgb = render_circle(data, scale_x, scale_y);
			my_pixel_put(&data->img_data, x, y, data->circle.rgb);	
		}
	}
	mlx_put_image_to_window(data->mlx, data->win, data->img_data.img, 0, 0);
	return (0);
}

void	my_pixel_put(t_img *img, int x, int y, int color)
{
	char	*pixel;

	pixel = img->addr + (y * img->line_len) + (x * (img->bpp / 8));
	*(unsigned int *)pixel = color;
}

int	render_circle(t_program *data, double column, double row)
{
	double	circle_cords[3] = {0.0, -1.0, 0.0};
	double	ray_direction[3] = {column, row, -1.0};
	double	ray_origin[3] = {0.0, 0.0, 2.0};
	double	radius = 0.5;

	(void)data;

	float b_correction = -2.0 * (ray_direction[0] * circle_cords[0] - ray_direction[1] * circle_cords[1] - ray_direction[2] * circle_cords[2]);
	float c_correction = -1.0 * (pow(circle_cords[0], 2) + pow(circle_cords[1], 2) + pow(circle_cords[0], 2)) - 2 * (ray_origin[0] * circle_cords[0] - ray_origin[1] * circle_cords[1] - ray_origin[2] * circle_cords[2]);

//	b_correction = 0;
//	c_correction = 0;
	
	float a = pow(ray_direction[0], 2) + pow(ray_direction[1], 2) + pow(ray_direction[2], 2);
	float b = 2.0 * (ray_direction[0] * ray_origin[0] + ray_direction[1] * ray_origin[1] + ray_direction[2] * ray_origin[2]) + b_correction;
	float c = pow(ray_origin[0], 2) + pow(ray_origin[1], 2) + pow(ray_origin[2], 2) - pow(radius, 2) + c_correction;


	
	float discriminant = (b * b) - (4 * a * c);

//	printf("%lf\n", discriminant);
	if (discriminant >= 0.0f)
		return (0xffff00ff);
	else
		return (0x00333333);
}

// Render circle
/*
	(bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0	

	(bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 + az^2 - r^2) = 0
	Where:
	a = Ray Origin
	b = Ray Direction
	r = radius
	t = Ray Travel Distance


	discriminant = b * b - 4 a c
	if (discriminant < 0) = 0 hits
	if (discriminant == 0) = 1 hit
	if (discriminant > 0) = 2 hits
*/

int	init_mlx(t_program *data)
{
	// NEED ERROR MANAGEMENT
	data->mlx = mlx_init();
	if (!data->mlx)
		exit(1);
	data->win = mlx_new_window(data->mlx, WIDTH, HEIGHT, "miniRT");
	if (!data->win)
		exit (1);
	data->img_data.img = mlx_new_image(data->mlx, WIDTH, HEIGHT);
	if (!data->img_data.img)
		exit (1);
	data->img_data.addr = mlx_get_data_addr(data->img_data.img, &data->img_data.bpp, &data->img_data.line_len, &data->img_data.endian);
	if (!data->img_data.addr)
		exit (1);
	return (0);
}

int	init_data(t_program *data)
{
	data->circle.radius = 10.0;
	data->circle.rgb = 0x00ff0000;
	data->circle.cords[0] = 0.0;
	data->circle.cords[1] = 0.0;
	return (0);
}

int	key_handler(int keysym, t_program *data)
{
	if (keysym == XK_Escape)
		clean_exit(data);
	else
		printf("stop it\n");
	return (1);
}

// DELETE
void	print_list(t_types *lst)
{
	t_cylinder	*cyl;
	t_sphere	*sphere;

	
	while (lst)
	{
		if (lst->type == 'y')
		{
			cyl = (t_cylinder *)lst;
			printf("\nCYLINDER\n");
			printf("coords: %f,%f,%f\n", cyl->coords[0], cyl->coords[1], cyl->coords[2]);
			printf("vector: %f,%f,%f\n", cyl->vector[0], cyl->vector[1], cyl->vector[2]);
			printf("radius: %f\n", cyl->radius);
			printf("height: %f\n", cyl->height);
			printf("rgb: %d,%d,%d\n", cyl->rgb[0], cyl->rgb[1], cyl->rgb[2]);
		}
		else if (lst->type == 's')
		{
			sphere = (t_sphere *)lst;
			printf("\nSPHERE\n");
			printf("coords: %f,%f,%f\n", sphere->coords[0], sphere->coords[1], sphere->coords[2]);
			printf("radius: %f\n", sphere->radius);
			printf("rgb: %d,%d,%d\n", sphere->rgb[0], sphere->rgb[1], sphere->rgb[2]);
		}
		lst = lst->next;
	}
}


int	clean_exit(t_program *data)
{
	print_list(data->objects);
	ft_lstclear(data->objects);
	mlx_destroy_image(data->mlx, data->img_data.img);
	mlx_destroy_window(data->mlx, data->win);
	mlx_destroy_display(data->mlx);
	free(data->mlx);
	exit (1);
}