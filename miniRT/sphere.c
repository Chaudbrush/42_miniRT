/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zali <zali@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:25:11 by vloureir          #+#    #+#             */
/*   Updated: 2025/12/10 14:38:43y zali             ###   ########.fr       */
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

int closest_vec(int *colors, t_vec3 **vecs, int total, t_vec3 camera)
{
	float	tdist;
	float	dist;
	int		smallest_idx;
	int		i;
	t_vec3	*vec;
	i = 0;
	dist = 99999;
	smallest_idx = 0;
	while (i < total)
	{
		if (colors[i] == -1)
		{
			i++;
			continue ;
		}
		vec = vecs[i];
		tdist = sqrt(
		    (camera.x - vec->x) * (camera.x - vec->x) +
		    (camera.y - vec->y) * (camera.y - vec->y) +
		    (camera.z - vec->z) * (camera.z - vec->z)
			);
		//tdist = sqrt(((camera.x - vec->x)*(camera.x - vec->x)) + ((camera.y + vec->y)*(camera.y + vec->y)) + ((camera.x - vec->z)*(camera.x - vec->z)));
		if (tdist < dist)
		{
			dist = tdist;
			smallest_idx = i;
		}
		i++;
	}
	if (colors[smallest_idx] == -1)
		return (0);
	return (colors[smallest_idx]);
}

void	my_pixel_put(t_img *img, int x, int y, int color)
{
	char	*pixel;

	pixel = img->addr + (y * img->line_len) + (x * (img->bpp / 8));
	*(unsigned int *)pixel = color;
}

int rt(t_program *data, t_vec3 ray_dir)
{
    t_types *obj = data->objects;
    t_vec3 origin = data->camera.coords;

    float nearest_t = 1e9f;
    int final_color = to_rgb(data->ambient.color);

    while (obj)
    {
//        float t = -1.0;
		data->hitpoint = -1;
        int color = 0;

        if (obj->type == 's')
            color = raytrace_sphere(ray_dir, (t_sphere *)obj, data->light.coords, data); 
        else if (obj->type == 'y')
            color = raytrace_cylinder(ray_dir, (t_cylinder *)obj, data->light.coords, data);
		else if (obj->type == 'p')
			color = raytrace_plane(ray_dir, (t_plane *)obj, data->light.coords, data);
	  	if (data->hitpoint > 0 && data->hitpoint < nearest_t && color != -1)
	  	{
			nearest_t = data->hitpoint;
			final_color = color;
		}
        obj = obj->next;
    }
    return final_color;
}

int raytrace_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light, t_program *data)
{
	t_vec3 origin = cyl->coords;
    float radius = cyl->radius;
    float height = cyl->height;
	t_vec3 ray_origin = {0,0,0};

	t_vec3 oc = { ray_origin.x - origin.x, ray_origin.y - origin.y, ray_origin.z - origin.z };
    // Cylinder aligned along Y-axis now
    float a = dir.x * dir.x + dir.z * dir.z;
    float b = 2.0 * (oc.x * dir.x + oc.z * dir.z);
    float c = oc.x * oc.x + oc.z * oc.z - radius * radius;

    float discriminant = b*b - 4*a*c;
    if (discriminant < 0.0)
        return -1; // no hit

    float sqrtD = sqrt(discriminant);
    float t1 = (-b - sqrtD) / (2*a);
    float t2 = (-b + sqrtD) / (2*a);

	float t = (t1 > 0) ? t1 : t2;
    if (t < 0) return -1;

	data->hitpoint = t;

	t_vec3 hit = { ray_origin.x + dir.x*t, ray_origin.y + dir.y*t, ray_origin.z + dir.z*t };

    // Normal at hitpoint
    t_vec3 normal = { hit.x - origin.x, hit.y - origin.y, hit.z - origin.z };
    float nl = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
    normal.x /= nl; normal.y /= nl; normal.z /= nl;

    // Normalize light
    float ll = sqrtf(light.x*light.x + light.y*light.y + light.z*light.z);
    t_vec3 L = { light.x/ll, light.y/ll, light.z/ll };

	int final_color = phong_color(data, cyl->color, normal, hit, L);

    // // Diffuse
    // float diffuse = - (normal.x*L.x + normal.y*L.y + normal.z*L.z);
    // if (diffuse < 0) diffuse = 0x00000000;

    // // Specular
    // t_vec3 view = { -hit.x, -hit.y, -hit.z }; // camera at origin
    // float vl = sqrtf(view.x*view.x + view.y*view.y + view.z*view.z);
    // view.x /= vl; view.y /= vl; view.z /= vl;

    // // reflect(L, N) = L - 2*dot(L,N)*N
    // float dotLN = L.x*normal.x + L.y*normal.y + L.z*normal.z;
    // t_vec3 reflect = { L.x - 2*dotLN*normal.x, L.y - 2*dotLN*normal.y, L.z - 2*dotLN*normal.z };

    // float spec = reflect.x*view.x + reflect.y*view.y + reflect.z*view.z;
    // if (spec < 0) spec = 0;
    // float shininess = 50.0f;
    // spec = powf(spec, shininess);
    // float spec_strength = 0.5f;
	// float r = cyl->color.x;
	// float g = cyl->color.y;
	// b = cyl->color.z;

    // // Apply diffuse
    // r *= diffuse; g *= diffuse; b *= diffuse;

    // // Add specular (white)
    // r += spec_strength*spec;
    // g += spec_strength*spec;
    // b += spec_strength*spec;

    // Check both intersections
    float y1 = oc.y + t1 * dir.y;
    float y2 = oc.y + t2 * dir.y;

    // Cylinder extends from -height/2 to +height/2 along Y
    float half = height / 2.0;
    int yax = 0; // default: no hit
    if (y1 >= -half && y1 <= half)
        yax = 1;
    else if (y2 >= -half && y2 <= half)
        yax = 1;
    if (yax)
		return (final_color);
    return -1;
}

int raytrace_plane(t_vec3 dir, t_plane *plane, t_vec3 light, t_program *data)
{
	
	/*
	A plane is characterized by a point p0, indicating its distance from the world's origin, and a normal , which defines the plane's orientation.
	We can derive a vector on the plane from any point p on it by subtracting p0 from p.
	Since this resultant vector lies within the plane, it is perpendicular to the plane's normal.
	Leveraging the property that the dot product of two perpendicular vectors equals 0, we have (equation 1):

	(p - p0) * n = 0

	Likewise, a ray is described using the parametric form (equation 2):

	l0 + l * t = p

	l0 = ray origin
	l = ray direction
	t = ray distance to point

	Subestituting we get:

	((l0 + l * t) - p0) * n = 0

	So:

	t = ((p0 - l0) * n) / l * n

	It's worth noting that if the plane and ray are parallel (i.e., when approaches 0), they either perfectly coincide, offering an infinite number of solutions, or they do not intersect at all.
	In practical C++ implementations, we typically return false (indicating no intersection) when the denominator is less than a very small threshold.
	*/

	float	t;
	float	denominator;
	t_vec3	sub;
	t_vec3 ray_origin = {0,0,0};
	
	(void)light;
	sub = vec_sub(plane->coords, ray_origin);
	denominator = dot_product(plane->vector, dir);
	if (denominator > 1e-6)
	{
		t = (dot_product(sub, plane->vector)) / denominator;
		if (t < 0)
			return (-1);
		data->hitpoint = t;

    t_vec3 hit = { ray_origin.x + dir.x*t, ray_origin.y + dir.y*t, ray_origin.z + dir.z*t };

  	// Normalize light
	t_vec3	L = normalize_vector(light);
	return (phong_color(data, plane->color, normalize_vector(plane->vector), hit, L));


//	return (to_rgb(plane->color));
	}
	return (-1);
}

int raytrace_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, t_program *data)
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
	data->hitpoint = t;
    // Hitpoint
    t_vec3 hit = { ray_origin.x + dir.x*t, ray_origin.y + dir.y*t, ray_origin.z + dir.z*t };

    // Normal at hitpoint
    t_vec3 normal = { hit.x - center.x, hit.y - center.y, hit.z - center.z };
	normal = normalize_vector(normal);

    // Normalize light
	t_vec3	L = normalize_vector(light);

	return (phong_color(data, sphere->color, normal, hit, L)); // Do we really need to pass light? Cant I normalize the light inside data and we use it from there?

    // // Diffuse
	// float diffuse = -dot_product(normal, L);
    // if (diffuse < 0) diffuse = 0x00000000;

    // // Specular
    // t_vec3 view = { -hit.x, -hit.y, -hit.z }; // camera at origin
	// view = normalize_vector(view);

	// float dotLN = dot_product(L, normal);
    // t_vec3 reflect = { L.x - 2*dotLN*normal.x, L.y - 2*dotLN*normal.y, L.z - 2*dotLN*normal.z };

	// float	spec = dot_product(reflect, view);
    // if (spec < 0) spec = 0;
    // float shininess = 100.0f;
    // spec = powf(spec, shininess);
    // float spec_strength = 0.5f;

	// float r = sphere->color.x;
	// float g = sphere->color.y;
	// b = sphere->color.z;

    // // Apply diffuse
    // r *= diffuse; g *= diffuse; b *= diffuse;

    // // Add specular (white)
    // r += spec_strength*spec;
    // g += spec_strength*spec;
    // b += spec_strength*spec;

    // // Clamp
    // if (r>1) r=1; if (g>1) g=1; if (b>1) b=1;

    // // Convert to int RGB
    // int ir = (int)(r*255);
    // int ig = (int)(g*255);
    // int ib = (int)(b*255);

    // return (ir<<16)|(ig<<8)|ib;
}

int	phong_color(t_program *data, t_vec3 color, t_vec3 normal, t_vec3 hit, t_vec3 light)
{
	/*
	Phong Color Model

	color = Ambient + Md * Diffuse + Ms * Specular

	multiply each one of them by the object color !

	Md = Material Diffuse
	Ms = Material Specular

	Ambient = light_color * light_strength * object_color

	Diffuse = dot_product(light_direction, surface_normal) * object_color

	Specular = pow(dot(reflected_light_direction, view_direction), shininess) * object_color

	typedef struct s_material
	{
		float	ambience;
		float	diffuse;
		float	specular;
		float	shininess;
	}	t_material;

	*/
	t_vec3	ambient;
	t_vec3	diffuse;
	t_vec3	specular = {0.0};

	ambient = vec_mult(data->ambient.color, color);
	float diffuse_dot = -dot_product(normal, light);
	if (diffuse_dot < 0)
		diffuse_dot = 0;
	diffuse = vec_scale(color, diffuse_dot);

	// Organize this better

	t_vec3 view = { -hit.x, -hit.y, -hit.z }; // camera at origin
	view = normalize_vector(view);

	float	shininess = 50.0f;
	float	spec_strength = 0.5f;
	float dotLN = dot_product(light, normal);
	t_vec3 reflect = { light.x - 2*dotLN*normal.x, light.y - 2*dotLN*normal.y, light.z - 2*dotLN*normal.z };

	float spec = dot_product(reflect, view);
	if (spec < 0)
		spec = 0;
	float specular_coeff = pow(spec, shininess) * spec_strength;
	specular = vec_scale(color, specular_coeff);

	return (to_rgb(vec_add(vec_add(ambient, diffuse), specular)));
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

float	dot_product(t_vec3 v1, t_vec3 v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
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
	printf("ambient: %f, %f, %f", data->ambient.color.x, data->ambient.color.y, data->ambient.color.z);
	print_list(data->objects); // DELETE
	ft_lstclear(data->objects);
	mlx_destroy_image(data->mlx, data->img_data.img);
	mlx_destroy_window(data->mlx, data->win);
	mlx_destroy_display(data->mlx);
	free(data->mlx);
	exit (1);
}
