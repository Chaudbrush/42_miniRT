#include "../minirt.h"

t_vec3	perp_component(t_vec3 v, t_vec3 axis)
{
	t_vec3	result;
	float	dot;

	dot = dot_product(v, axis);
	result.x = v.x - dot * axis.x;
	result.y = v.y - dot * axis.y;
	result.z = v.z - dot * axis.z;
	return (result);
}

static float	quadratic_helper(t_vec3 d_perp, t_vec3 oc_perp, float radius)
{
	float	a;
	float	b;
	float	c;
	float	t;
	float	sqrt_h;

	a = d_perp.x * d_perp.x + d_perp.y * d_perp.y + d_perp.z * d_perp.z;
	b = 2.0f * (d_perp.x * oc_perp.x + d_perp.y * oc_perp.y
			+ d_perp.z * oc_perp.z);
	c = oc_perp.x * oc_perp.x + oc_perp.y * oc_perp.y
		+ oc_perp.z * oc_perp.z - radius * radius;
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

float	cylinder_quadratic(t_vec3 ray_origin, t_vec3 dir, t_cylinder *cyl)
{
	t_vec3	oc;
	t_vec3	d_perp;
	t_vec3	oc_perp;

	oc = vec_sub(ray_origin, cyl->coords);
	d_perp = perp_component(dir, cyl->vector);
	oc_perp = perp_component(oc, cyl->vector);
	return (quadratic_helper(d_perp, oc_perp, cyl->radius));
}

t_vec3	cylinder_normal(t_cylinder *cyl, t_vec3 hit)
{
	t_vec3	normal;
	float	hit_dot_a;
	float	length;

	hit_dot_a = (hit.x - cyl->coords.x) * cyl->vector.x + (hit.y
			- cyl->coords.y) * cyl->vector.y + (hit.z - cyl->coords.z)
		* cyl->vector.z;
	normal.x = hit.x - cyl->coords.x - hit_dot_a * cyl->vector.x;
	normal.y = hit.y - cyl->coords.y - hit_dot_a * cyl->vector.y;
	normal.z = hit.z - cyl->coords.z - hit_dot_a * cyl->vector.z;
	length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z
			* normal.z);
	normal.x /= length;
	normal.y /= length;
	normal.z /= length;
	return (normal);
}
