/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zali <zali@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:25:11 by vloureir          #+#    #+#             */
/*   Updated: 2025/12/10 11:54:13 by zali             ###   ########.fr       */
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
	//t_vec3	origin_cylinder = {1, 0, -2};
	//t_vec3	origin_sphere = {0, 0, -1};
	t_types *object;
	object = data->objects;
	while (object)
	{
		if (object->type == 's')
		{
			t_sphere *sphere = (t_sphere *)object;
			int color = raytrace_sphere(vec, sphere, data->light.coords);
			if (color != -1)
				return (color);
		}
/*
	if (object->type == 'y')
	{
		t_cylinder *cyl = (t_cylinder *) object;
		return raytrace_cylinder(vec, cyl->coords, data->light.coords);
	}
*/
		object = object->next;
	}
	//return (0xFFFFFFFF);
	return 0;
}

int raytrace_cylinder(t_vec3 dir, t_vec3 origin, t_vec3 light)
{
	(void) light;
    float radius = 0.5;
   // float height = 1.0f;

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
	if (t1 || t2)	return (0xFFFF0000);
	return 0;
	/*
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

    t_vec3 p = vec_sub(hit, origin); 

    t_vec3 normal = { p.x, 0, p.z };

    float nl = sqrt(normal.x*normal.x + normal.z*normal.z);
    normal.x /= nl;
    normal.z /= nl;

    float ll = sqrt(light.x*light.x + light.y*light.y + light.z*light.z);
    t_vec3 L = { light.x/ll, light.y/ll, light.z/ll };

    float lf = normal.x*(-L.x) + normal.y*(-L.y) + normal.z*(-L.z);
    if (lf < 0) lf = 0;

    int base = 0xFF0000;

    float r = ((base>>16)&0xFF)/255.0f * lf;
    float g = ((base>>8 )&0xFF)/255.0f * lf;
    float b2= ((base    )&0xFF)/255.0f * lf;

    return ((int)(r*255)<<16) | ((int)(g*255)<<8) | (int)(b2*255);
*/
}

int raytrace_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light)
{
	t_vec3 center = sphere->coords;
	float radius = sphere->radius;
    t_vec3 ray_origin = {0,0,0};


    // Quadratic coefficients
    t_vec3 oc = { ray_origin.x - center.x, ray_origin.y - center.y, ray_origin.z - center.z };
    float a = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
    float b = 2.0f * (oc.x*dir.x + oc.y*dir.y + oc.z*dir.z);
    float c = oc.x*oc.x + oc.y*oc.y + oc.z*oc.z - radius*radius;

    float disc = b*b - 4*a*c;
    if (disc < 0.0f) return -1; // no hit

    float sqrtD = sqrtf(disc);
    float t0 = (-b - sqrtD) / (2*a);
    float t1 = (-b + sqrtD) / (2*a);

    // Pick nearest positive intersection
    float t = (t0 > 0) ? t0 : t1;
    if (t < 0) return 0xFFFF0000;

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

	float r = sphere->color.x;
	float g = sphere->color.y;
	b = sphere->color.z;

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
// VECTOR FUNCTIONS
t_vec3	normalize_vector(t_vec3 vector)
{
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

int to_rgb(t_vec3 c)
{

    int r = (int)(fminf(fmaxf(c.x, 0.0f), 1.0f) * 255.0f);
    int g = (int)(fminf(fmaxf(c.y, 0.0f), 1.0f) * 255.0f);
    int b = (int)(fminf(fmaxf(c.z, 0.0f), 1.0f) * 255.0f);
    return ((r << 16) | (g << 8) | b);
}

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