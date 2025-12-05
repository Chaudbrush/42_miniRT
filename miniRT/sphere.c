/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:25:11 by vloureir          #+#    #+#             */
/*   Updated: 2025/12/05 13:31:48 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"

int	main(int argc, char **argv)
{
	t_program	data;
	
	if (parse_data(&data, argc, argv) || init_mlx(&data))
		exit (1); // Check errors
	mlx_loop_hook(data.mlx, render, &data);
	mlx_hook(data.win, DestroyNotify, NoEventMask, clean_exit, &data);
	mlx_hook(data.win, KeyPress, KeyPressMask, key_handler, &data);
	mlx_loop(data.mlx);
}

int	render(t_program *data)
{
	int		color;
	float	x;
	float	y;
	double	scaled_x;
	double	scaled_y;
	float	aspect;

	y = -1.0;
	while (++y < HEIGHT)
	{
		x = -1.0;
		while (++x < WIDTH)
		{

			if (WIDTH > HEIGHT)
				aspect = (float)WIDTH / (float)HEIGHT;	
			else
				aspect = (float)HEIGHT / (float)WIDTH; 
			scaled_x = (2.0 * (x /	WIDTH)) - 1.0;
			scaled_y = (2.0 * (y /	HEIGHT)) - 1.0;
			if (WIDTH > HEIGHT)
				scaled_x *= aspect;
			else
				scaled_y *= aspect;
			color = rt(data, (t_vec3){scaled_x, scaled_y, -1.0f});
			my_pixel_put(&data->img_data, x, y, color);	
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

int	rt(t_program *data, t_vec3 vec)
{
	t_vec3	origin_cylinder = {1, 0, -2};
	t_vec3	origin_sphere = {0, 0, -1};
	
	// We need to iterate over the whole list here, checking for the forms, and rendering them when found
	int sphere_col = raytrace_sphere(vec, origin_sphere, data->light.coords);
	int cylinder_col = raytrace_cylinder(vec, origin_cylinder, data->light.coords);
	if (sphere_col && !cylinder_col)
		return (sphere_col);
	if (cylinder_col && !sphere_col)
		return (cylinder_col);
	if (sphere_col && cylinder_col)
	{
		if (origin_cylinder.z < origin_sphere.z)
			return (cylinder_col);
		else
			return (sphere_col);
	}
	return (0x001f1f1f);
}

// int	rt(t_program *data, t_vec3 vec)
// {
// 	t_vec3	origin_plane = {0, 1, -1};
	
// 	int	plane_col = raytrace_place(vec, origin_plane, data->light.coords);

// 	if (plane_col)
// 		return (plane_col);
// 	return (0x001f1f1f);
// }


// int	raytrace_plane(t_vec3 dir, t_vec3 origin, t_vec3 light)
// {
	
	

	
// }


int raytrace_cylinder(t_vec3 dir, t_vec3 origin, t_vec3 light)
{
    float radius = 0.5;
    float height = 1.0f;

    t_vec3 ray_origin = (t_vec3){0,0,0};

    // Convert ray origin to cylinder local space:
    t_vec3 oc = vec_sub(ray_origin, origin);

    float a = dir.x*dir.x + dir.z*dir.z;
    float b = 2 * (oc.x*dir.x + oc.z*dir.z);
    float c = oc.x*oc.x + oc.z*oc.z - radius*radius;

    float disc = b*b - 4*a*c;
    if (disc < 0) return 0;

    float sq = sqrtf(disc);
    float t1 = (-b - sq) / (2*a);
    float t2 = (-b + sq) / (2*a);

    float half = height/2;

    float t = -1;

    // Check t1
    float y1 = oc.y + t1*dir.y;
    if (t1 >= 0 && y1 >= -half && y1 <= half)
        t = t1;

    // Check t2
    float y2 = oc.y + t2*dir.y;
    if (t2 >= 0 && y2 >= -half && y2 <= half)
        if (t < 0 || t2 < t) t = t2;

    if (t < 0)
        return 0;

    // Hit point in world:
    t_vec3 hit = vec_add(ray_origin, vec_scale(dir, t));

    // Now compute normal in cylinder local space:
    t_vec3 p = vec_sub(hit, origin); // convert to local cylinder coords

    t_vec3 normal = { p.x, 0, p.z };

    float nl = sqrt(normal.x*normal.x + normal.z*normal.z);
    normal.x /= nl;
    normal.z /= nl;

    // Light
    float ll = sqrt(light.x*light.x + light.y*light.y + light.z*light.z);
    t_vec3 L = { light.x/ll, light.y/ll, light.z/ll };

    float lf = normal.x*(-L.x) + normal.y*(-L.y) + normal.z*(-L.z);
    if (lf < 0) lf = 0;

    // Base color (red)
    int base = 0xFF0000;

    float r = ((base>>16)&0xFF)/255.0f * lf;
    float g = ((base>>8 )&0xFF)/255.0f * lf;
    float b2= ((base    )&0xFF)/255.0f * lf;

    return ((int)(r*255)<<16) | ((int)(g*255)<<8) | (int)(b2*255);
}

int raytrace_sphere(t_vec3 dir, t_vec3 center, t_vec3 light)
{
    // Camera at origin
    t_vec3 ray_origin = {0,0,0};

    // Sphere radius
    float radius = 0.5f;

    // Quadratic coefficients
    t_vec3 oc = { ray_origin.x - center.x, ray_origin.y - center.y, ray_origin.z - center.z };
    float a = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
    float b = 2.0f * (oc.x*dir.x + oc.y*dir.y + oc.z*dir.z);
    float c = oc.x*oc.x + oc.y*oc.y + oc.z*oc.z - radius*radius;

    float disc = b*b - 4*a*c;
    if (disc < 0.0f) return 0x000000; // no hit

    float sqrtD = sqrtf(disc);
    float t0 = (-b - sqrtD) / (2*a);
    float t1 = (-b + sqrtD) / (2*a);

    // Pick nearest positive intersection
    float t = (t0 > 0) ? t0 : t1;
    if (t < 0) return 0x000000;

    // Hitpoint
    t_vec3 hit = { ray_origin.x + dir.x*t, ray_origin.y + dir.y*t, ray_origin.z + dir.z*t };

    // Normal at hitpoint
    t_vec3 normal = { hit.x - center.x, hit.y - center.y, hit.z - center.z };
    float nl = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
    normal.x /= nl; normal.y /= nl; normal.z /= nl;

    // Normalize light
    float ll = sqrtf(light.x*light.x + light.y*light.y + light.z*light.z);
    t_vec3 L = { light.x/ll, light.y/ll, light.z/ll };

    // Diffuse
    float diffuse = - (normal.x*L.x + normal.y*L.y + normal.z*L.z);
    if (diffuse < 0) diffuse = 0;

    // Specular
    t_vec3 view = { -hit.x, -hit.y, -hit.z }; // camera at origin
    float vl = sqrtf(view.x*view.x + view.y*view.y + view.z*view.z);
    view.x /= vl; view.y /= vl; view.z /= vl;

    // reflect(L, N) = L - 2*dot(L,N)*N
    float dotLN = L.x*normal.x + L.y*normal.y + L.z*normal.z;
    t_vec3 reflect = { L.x - 2*dotLN*normal.x, L.y - 2*dotLN*normal.y, L.z - 2*dotLN*normal.z };

    float spec = reflect.x*view.x + reflect.y*view.y + reflect.z*view.z;
    if (spec < 0) spec = 0;
    float shininess = 50.0f;
    spec = powf(spec, shininess);
    float spec_strength = 0.5f;

    // Base color
    int base = 0xFF5500;
    float r = ((base>>16)&0xFF)/255.0f;
    float g = ((base>>8 )&0xFF)/255.0f;
    b = ((base    )&0xFF)/255.0f;

    // Apply diffuse
    r *= diffuse; g *= diffuse; b *= diffuse;

    // Add specular (white)
    r += spec_strength*spec;
    g += spec_strength*spec;
    b += spec_strength*spec;

    // Clamp
    if (r>1) r=1; if (g>1) g=1; if (b>1) b=1;

    // Convert to int RGB
    int ir = (int)(r*255);
    int ig = (int)(g*255);
    int ib = (int)(b*255);

    return (ir<<16)|(ig<<8)|ib;
}

// int	render_circle(t_program *data, double column, double row)
// {
// 	double	circle_cords[3] = {0.0, 0.0, 0.0};
// 	double	ray_direction[3] = {column, row, -1.0};
// 	double	ray_origin[3] = {0.0, 0.0, 2.0};
// 	double	radius = 0.5;

// 	(void)data;

// 	float b_correction = -2.0 * (ray_direction[0] * circle_cords[0] - ray_direction[1] * circle_cords[1] - ray_direction[2] * circle_cords[2]);
// 	float c_correction = -1.0 * (pow(circle_cords[0], 2) + pow(circle_cords[1], 2) + pow(circle_cords[0], 2)) - 2 * (ray_origin[0] * circle_cords[0] - ray_origin[1] * circle_cords[1] - ray_origin[2] * circle_cords[2]);

// 	b_correction = 0;
// 	c_correction = 0;
	
// 	float a = pow(ray_direction[0], 2) + pow(ray_direction[1], 2) + pow(ray_direction[2], 2);
// 	float b = 2.0 * (ray_direction[0] * ray_origin[0] + ray_direction[1] * ray_origin[1] + ray_direction[2] * ray_origin[2]) + b_correction;
// 	float c = pow(ray_origin[0], 2) + pow(ray_origin[1], 2) + pow(ray_origin[2], 2) - pow(radius, 2) + c_correction;


	
// 	float discriminant = (b * b) - (4 * a * c);

// //	printf("%lf\n", discriminant);
// 	if (discriminant < 0.0f)
// 		return (0x001f1f1f);
// 	else
// 	{
// 		float	t = (-b - sqrt(discriminant) / 2.0f * a);

// 		t_vec3	color_vec;
// 		color_vec.x = ray_origin[0] + ray_direction[0] * t;
// 		color_vec.y = ray_origin[1] + ray_direction[1] * t;
// 		color_vec.z = ray_origin[2] + ray_direction[2] * t;
// 		unsigned int color = to_rgb(color_vec);
// 		return (color);


// 	}
// //	return ((t_vec3){255.0, 1.0, 12.4});
// //		return (0x00333333);
	
// }


// VECTOR FUNCTIONS
t_vec3	normalize_vector(t_vec3 vector)
{
	// To normalize a vector, you divide each coordinate by it's len
	float	len;
	t_vec3	new;

	len = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	new.x = vector.x / len;
	new.y = vector.y / len;
	new.z = vector.z / len;
	return (new);
}

t_vec3	vec_add(t_vec3 vec1, t_vec3 vec2)
{
	t_vec3	new;
	
	new.x = vec1.x + vec2.x;
	new.y = vec1.y + vec2.y;
	new.z = vec1.z + vec2.z;
	return (new);
}

t_vec3	vec_mult(t_vec3 vec1, t_vec3 vec2)
{
	t_vec3	new;
	
	new.x = vec1.x * vec2.x;
	new.y = vec1.y * vec2.y;
	new.z = vec1.z * vec2.z;
	return (new);
}

t_vec3	vec_sub(t_vec3 vec1, t_vec3 vec2)
{
	t_vec3	new;
	
	new.x = vec1.x - vec2.x;
	new.y = vec1.y - vec2.y;
	new.z = vec1.z - vec2.z;
	return (new);
}

t_vec3 vec_scale(t_vec3 vector, float s)
{
    return ((t_vec3){vector.x * s, vector.y * s, vector.z * s});
}



// vec3 must be in range [0, 1]
// unsigned int	get_color(t_vec3 colors)
// {
// 	unsigned int result;

// //	result = 0;
// 	result = (int)(colors.x * 255.99) << 16;
// 	result |= (int)(colors.y * 255.99) << 8;
// 	result |= (int)(colors.z * 255.99);
// 	return (result);
// }

int to_rgb(t_vec3 c)
{

    int r = (int)(fminf(fmaxf(c.x, 0.0f), 1.0f) * 255.0f);
    int g = (int)(fminf(fmaxf(c.y, 0.0f), 1.0f) * 255.0f);
    int b = (int)(fminf(fmaxf(c.z, 0.0f), 1.0f) * 255.0f);
    return ((r << 16) | (g << 8) | b);
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
	// NEED PROPER ERROR MANAGEMENT
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

int	key_handler(int keysym, t_program *data)
{
	if (keysym == XK_Escape)
		clean_exit(data);
	else
		return (1);
	return (1);
}

// DELETE
void	print_list(t_types *lst)
{
	t_cylinder	*cyl;
	t_sphere	*sphere;
	t_plane		*plane;

	while (lst)
	{
		if (lst->type == 'y')
		{
			cyl = (t_cylinder *)lst;
			printf("\nCYLINDER\n");
			printf("coords: %f,%f,%f\n", cyl->coords.x, cyl->coords.y, cyl->coords.z);
			printf("vector: %f,%f,%f\n", cyl->vector.x, cyl->vector.y, cyl->vector.z);
			printf("radius: %f\n", cyl->radius);
			printf("height: %f\n", cyl->height);
			printf("float rgb: %f,%f,%f\n", cyl->color.x, cyl->color.y, cyl->color.z);
			printf("rgb: %d,%d,%d\n", (int)(cyl->color.x * 255.99), (int)(cyl->color.y * 255.99), (int)(cyl->color.z * 255.99));
		}
		else if (lst->type == 's')
		{
			sphere = (t_sphere *)lst;
			printf("\nSPHERE\n");
			printf("coords: %f,%f,%f\n", sphere->coords.x, sphere->coords.y, sphere->coords.z);
			printf("radius: %f\n", sphere->radius);
			printf("float rgb: %f,%f,%f\n", sphere->color.x, sphere->color.y, sphere->color.z);
			printf("rgb: %d,%d,%d\n", (int)(sphere->color.x * 255.99), (int)(sphere->color.y * 255.99), (int)(sphere->color.z * 255.99));
		}
		else if (lst->type == 'p')
		{
			plane = (t_plane *)lst;
			printf("\nPLANE\n");
			printf("coords: %f,%f,%f\n", plane->coords.x, plane->coords.y, plane->coords.z);
			printf("vector: %f,%f,%f\n", plane->vector.x, plane->vector.y, plane->vector.z);
			printf("float rgb: %f,%f,%f\n", plane->color.x, plane->color.y, plane->color.z);\
			printf("rgb: %d,%d,%d\n", (int)(plane->color.x * 255.99), (int)(plane->color.y * 255.99), (int)(plane->color.z * 255.99));
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