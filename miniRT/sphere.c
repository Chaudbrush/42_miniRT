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
    int final_color = 0;

    while (obj)
    {
        float t = -1.0;
        int color = 0;

        if (obj->type == 's')
            color = raytrace_sphere(ray_dir, (t_sphere *)obj, data->light.coords, &t); 
        else if (obj->type == 'y')
            color = raytrace_cylinder(ray_dir, (t_cylinder *)obj, data->light.coords, &t); 
	  	if (t > 0 && t < nearest_t && color != -1)
	  	{
			nearest_t = t;
			final_color = color;
		}
        obj = obj->next;
    }

    return final_color;
}

/*
int rt(t_program *data, t_vec3 ray_dir)
{
    t_types *object;
    int i;
    int colors[5];
    t_vec3 *centers[5];
	float nearest_t = 1000000000.0f;

    i = 0;
    object = data->objects;
    while (object && i < 5)
    {
        if (object->type == 'y') // cylinder
        {
            colors[i] = raytrace_cylinder(ray_dir, (t_cylinder *)object, data->light.coords);
            centers[i] = &(((t_cylinder *)object)->coords);
        }
        else if (object->type == 's') // sphere
        {
            colors[i] = raytrace_sphere(ray_dir, (t_sphere *)object, data->light.coords);
            centers[i] = &(((t_sphere *)object)->coords);
        }
        else if (object->type == 'p') // plane (if you want center-like point)
        {
            colors[i] = raytrace_plane(ray_dir, (t_plane *)object, data->light.coords);
            centers[i] = &(((t_plane *)object)->coords);
        }
        else
        {
            colors[i] = -1;
            centers[i] = NULL;
        }
        i++;
        object = object->next;
    }
    // i is now number of objects processed
    return (closest_vec(colors, centers, --i, data->camera.coords));
}
*/

int raytrace_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light, float *hit_point)
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

	*hit_point = t;

	t_vec3 hit = { ray_origin.x + dir.x*t, ray_origin.y + dir.y*t, ray_origin.z + dir.z*t };

    // Normal at hitpoint
    t_vec3 normal = { hit.x - origin.x, hit.y - origin.y, hit.z - origin.z };
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
	float r = cyl->color.x;
	float g = cyl->color.y;
	b = cyl->color.z;

    // Apply diffuse
    r *= diffuse; g *= diffuse; b *= diffuse;

    // Add specular (white)
    r += spec_strength*spec;
    g += spec_strength*spec;
    b += spec_strength*spec;

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
	{
		if (r>1) r=1; if (g>1) g=1; if (b>1) b=1;
	    int ir = (int)(r*255);
	    int ig = (int)(g*255);
	    int ib = (int)(b*255);

	    return (ir<<16)|(ig<<8)|ib;
	}
    return -1;
}

int raytrace_plane(t_vec3 dir, t_plane *plane, t_vec3 light)
{
	
	/*
	Definition:
	
	C is a point that lies on the plane
	V is the plane normal (unit length)

	To hit a plane we notice that:

		(P-C)|V = 0

	This means that the P-C vector is perpendicular to the normal, which is true when the point P lies on the plane.

	Solution:

		(D*t+X)|V = 0
		D|V*t = -X|V
		t = -X|V / D|V

	Before the division we should check whether D|V is nonzero. We can also check if the signs of D|V and X|V are different (if not, resulting t will be negative).

	Surface normal vector at point P equals to the plane normal, unless D|V is negative, in which case N=-V. 
	
	
	X:O - C - where O is the ray origin, C is the center of the shape we hit
	V: the plane normal
	D: ray direction

	C = origin;
	V = data->
	
	D = dir;
	0 = ray_origin;



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
//	printf("denom: %f\n", denominator);
	if (denominator > 1e-6)
	{
		t = (dot_product(sub, plane->vector)) / denominator;
//		printf("t: %f\n", t);
		if (t >= 0)
			return (0xFFFF00FF);
	}
	return (0);
}

int raytrace_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, float *hit_point)
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
	*hit_point = t;
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
	print_list(data->objects);
	ft_lstclear(data->objects);
	mlx_destroy_image(data->mlx, data->img_data.img);
	mlx_destroy_window(data->mlx, data->win);
	mlx_destroy_display(data->mlx);
	free(data->mlx);
	exit (1);
}