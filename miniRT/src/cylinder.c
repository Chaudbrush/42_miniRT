#include "../minirt.h"

float	cylinder_cap(t_cylinder *cyl, t_vec3 ray_origin, t_vec3 dir, int top)
{
	t_vec3	center;
	t_vec3	hit;
	t_vec3	v_perp;
	float	denom;
	float	t_cap;

	center.x = cyl->coords.x + cyl->vector.x * (top * 2 - 1) * (cyl->height
			* 0.5f);
	center.y = cyl->coords.y + cyl->vector.y * (top * 2 - 1) * (cyl->height
			* 0.5f);
	center.z = cyl->coords.z + cyl->vector.z * (top * 2 - 1) * (cyl->height
			* 0.5f);
	denom = dot_product(dir, cyl->vector);
	if (denom < 1e-6f && denom > -1e-6f)
		return (-1);
	t_cap = (dot_product(vec_sub(center, ray_origin), cyl->vector)) / denom;
	if (t_cap <= 0.0f)
		return (-1);
	hit = vec_add(ray_origin, vec_scale(dir, t_cap));
	v_perp = perp_component(vec_sub(hit, center), cyl->vector);
	if ((v_perp.x * v_perp.x + v_perp.y * v_perp.y + v_perp.z
			* v_perp.z) <= cyl->radius * cyl->radius)
		return (t_cap);
	return (-1);
}

int	hit_inside_cylinder(t_cylinder *cyl, t_vec3 ray_origin, t_vec3 dir, float t)
{
	t_vec3	oc;
	float	oc_dot_a;
	float	d_dot_a;
	float	hit_h;
	float	half;

	oc = vec_sub(ray_origin, cyl->coords);
	oc_dot_a = dot_product(oc, cyl->vector);
	d_dot_a = dot_product(dir, cyl->vector);
	oc = vec_sub(ray_origin, cyl->coords);
	hit_h = oc_dot_a + t * d_dot_a;
	half = cyl->height * 0.5f;
	return (hit_h >= -half && hit_h <= half);
}

int	cap_color(t_vec3 dir, t_cylinder *cyl, t_vec3 light, t_program *data)
{
	t_vec3	hit;
	t_vec3	l_dir;
	float	t[2];
	int		i;

	i = 0;
	t[0] = cylinder_cap(cyl, data->camera.coords, dir, 1);
	t[1] = cylinder_cap(cyl, data->camera.coords, dir, 0);
	while (i < 2)
	{
		if (t[i] > 0.0f)
		{
			hit = vec_add(data->camera.coords, vec_scale(dir, t[i]));
			l_dir = normalize_vector(vec_sub(hit, light));
			data->hitpoint = t[i];
			if (i == 1)
				return (get_color(data, cyl->color, vec_scale(cyl->vector, -1),
						l_dir));
			return (get_color(data, cyl->color, cyl->vector, l_dir));
		}
		i++;
	}
	return (-1);
}

int	shadow_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light,
		t_vec3 ray_origin)
{
	float	t;
	float	light_dist;
	t_vec3	d;

	d = vec_sub(ray_origin, light);
	light_dist = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
	t = cylinder_quadratic(ray_origin, dir, cyl);
	if (t > 0.0f && hit_inside_cylinder(cyl, ray_origin, dir, t)
		&& t < light_dist)
		return (1);
	t = cylinder_cap(cyl, ray_origin, dir, 1);
	if (t > 0.0f && t < light_dist)
		return (1);
	t = cylinder_cap(cyl, ray_origin, dir, 0);
	if (t > 0.0f && t < light_dist)
		return (1);
	return (0);
}

int	raytrace_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light,
		t_program *data)
{
	t_vec3	ray_origin;
	float	t;
	t_vec3	hit;
	t_vec3	l_dir;

	ray_origin = data->camera.coords;
	t = cylinder_quadratic(ray_origin, dir, cyl);
	if (t > 0.0f && hit_inside_cylinder(cyl, ray_origin, dir, t))
	{
		data->hitpoint = t;
		hit = vec_add(ray_origin, vec_scale(dir, t));
		l_dir = normalize_vector(vec_sub(hit, light));
		if (check_shadow(data, hit, vec_scale(l_dir, -1.0), cyl->index))
			return (to_rgb(vec_mult(data->ambient.color, cyl->color)));
		else
			return (get_color(data, cyl->color, cylinder_normal(cyl, hit),
					l_dir));
	}
	return (cap_color(dir, cyl, light, data));
}
