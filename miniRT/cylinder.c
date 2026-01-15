/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:13:27 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/15 16:14:49 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int raytrace_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light, t_program *data)
{
    t_vec3 origin = cyl->coords;
    t_vec3 axis   = cyl->vector;  // normalized
    float radius  = cyl->radius;
    float height  = cyl->height;
	t_vec3	ray_origin = data->camera.coords;

    // Vector from cylinder center to ray origin
    t_vec3 oc = {
        ray_origin.x - origin.x,
        ray_origin.y - origin.y,
        ray_origin.z - origin.z
    };

    // Project ray and oc onto cylinder axis 
    float d_dot_a  = dir.x*axis.x + dir.y*axis.y + dir.z*axis.z;
    float oc_dot_a = oc.x*axis.x + oc.y*axis.y + oc.z*axis.z;

    // Perpendicular vector -> calc
    t_vec3 d_perp = { dir.x - d_dot_a*axis.x, dir.y - d_dot_a*axis.y, dir.z - d_dot_a*axis.z };
    t_vec3 oc_perp = { oc.x - oc_dot_a*axis.x, oc.y - oc_dot_a*axis.y, oc.z - oc_dot_a*axis.z };

    // quadratic
    float a = d_perp.x*d_perp.x + d_perp.y*d_perp.y + d_perp.z*d_perp.z;
    float b = 2.0f * (d_perp.x*oc_perp.x + d_perp.y*oc_perp.y + d_perp.z*oc_perp.z);
    float c = oc_perp.x*oc_perp.x + oc_perp.y*oc_perp.y + oc_perp.z*oc_perp.z - radius*radius;

    float disc = b*b - 4*a*c;
    if (disc < 0.0f) return -1; // no hit

    float sqrtD = sqrtf(disc);
    float t1 = (-b - sqrtD)/(2*a);
    float t2 = (-b + sqrtD)/(2*a);
    float t = (t1 > 0.0f) ? t1 : t2;
    if (t <= 0.0f) return -1;

    // Check body height
    float hit_h = oc_dot_a + t*d_dot_a;
    float half = height*0.5f;
    t_vec3 hit = { ray_origin.x + dir.x*t, ray_origin.y + dir.y*t, ray_origin.z + dir.z*t };
	t_vec3	L_dir = normalize_vector(vec_sub(hit, light));
	
    if (hit_h >= -half && hit_h <= half)
    {
        data->hitpoint = t;

        // Normal
        float hit_dot_a = (hit.x-origin.x)*axis.x + (hit.y-origin.y)*axis.y + (hit.z-origin.z)*axis.z;
        t_vec3 normal = { hit.x-origin.x - hit_dot_a*axis.x,
                          hit.y-origin.y - hit_dot_a*axis.y,
                          hit.z-origin.z - hit_dot_a*axis.z };
        float nl = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        normal.x /= nl; normal.y /= nl; normal.z /= nl;

        // // Light normalization
        // float ll = sqrtf(light.x*light.x + light.y*light.y + light.z*light.z);
        // t_vec3 L = { light.x/ll, light.y/ll, light.z/ll };

		

        int final_color;
        // if (check_shadow(data, hit, vec_scale(L,-1.0), cyl->index))
        //     final_color = to_rgb(vec_mult(data->ambient.color, cyl->color));
        // else
            final_color = get_color(data, cyl->color, normal, L_dir);

        return final_color;
    }

    // cap top
    t_vec3 top_center = { origin.x + axis.x*half, origin.y + axis.y*half, origin.z + axis.z*half };
    float denom = dir.x*axis.x + dir.y*axis.y + dir.z*axis.z;
    if (fabs(denom) > 1e-6f)
    {
        float t_cap = ((top_center.x - ray_origin.x)*axis.x +
                       (top_center.y - ray_origin.y)*axis.y +
                       (top_center.z - ray_origin.z)*axis.z) / denom;
        if (t_cap > 0.0f)
        {
            t_vec3 hit_cap = { ray_origin.x + dir.x*t_cap, ray_origin.y + dir.y*t_cap, ray_origin.z + dir.z*t_cap };
            t_vec3 v = { hit_cap.x-top_center.x, hit_cap.y-top_center.y, hit_cap.z-top_center.z };
            t_vec3 v_perp = { v.x - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.x,
                              v.y - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.y,
                              v.z - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.z };
            float dist2 = v_perp.x*v_perp.x + v_perp.y*v_perp.y + v_perp.z*v_perp.z;
            if (dist2 <= radius*radius)
            {
                data->hitpoint = t_cap;
                return get_color(data, cyl->color, axis, L_dir); // normal = axis
            }
        }
    }

    // cap bot
    t_vec3 bottom_center = { origin.x - axis.x*half, origin.y - axis.y*half, origin.z - axis.z*half };
    if (fabs(denom) > 1e-6f)
    {
        float t_cap = ((bottom_center.x - ray_origin.x)*axis.x +
                       (bottom_center.y - ray_origin.y)*axis.y +
                       (bottom_center.z - ray_origin.z)*axis.z) / denom;
        if (t_cap > 0.0f)
        {
            t_vec3 hit_cap = { ray_origin.x + dir.x*t_cap, ray_origin.y + dir.y*t_cap, ray_origin.z + dir.z*t_cap };
            t_vec3 v = { hit_cap.x-bottom_center.x, hit_cap.y-bottom_center.y, hit_cap.z-bottom_center.z };
            t_vec3 v_perp = { v.x - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.x,
                              v.y - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.y,
                              v.z - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.z };
            float dist2 = v_perp.x*v_perp.x + v_perp.y*v_perp.y + v_perp.z*v_perp.z;
            if (dist2 <= radius*radius)
            {
                data->hitpoint = t_cap;
                return get_color(data, cyl->color, vec_scale(axis,-1), L_dir); // normal = -axis
            }
        }
    }

    return -1; // no hit
}


int shadow_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light, t_vec3 ray_origin)
{
    (void)light;

    t_vec3 origin = cyl->coords;
    t_vec3 axis   = cyl->vector;
    float radius  = cyl->radius;
    float height  = cyl->height;

    t_vec3 oc = { ray_origin.x - origin.x, ray_origin.y - origin.y, ray_origin.z - origin.z };

    float d_dot_a  = dir.x*axis.x + dir.y*axis.y + dir.z*axis.z;
    float oc_dot_a = oc.x*axis.x + oc.y*axis.y + oc.z*axis.z;

    t_vec3 d_perp = { dir.x - d_dot_a*axis.x, dir.y - d_dot_a*axis.y, dir.z - d_dot_a*axis.z };
    t_vec3 oc_perp = { oc.x - oc_dot_a*axis.x, oc.y - oc_dot_a*axis.y, oc.z - oc_dot_a*axis.z };

    float a = d_perp.x*d_perp.x + d_perp.y*d_perp.y + d_perp.z*d_perp.z;
    float b = 2.0f * (d_perp.x*oc_perp.x + d_perp.y*oc_perp.y + d_perp.z*oc_perp.z);
    float c = oc_perp.x*oc_perp.x + oc_perp.y*oc_perp.y + oc_perp.z*oc_perp.z - radius*radius;

    float disc = b*b - 4*a*c;
    if (disc >= 0.0f)
    {
        float sqrtD = sqrtf(disc);
        float t1 = (-b - sqrtD)/(2*a);
        float t2 = (-b + sqrtD)/(2*a);
        float t = (t1 > 0.0f) ? t1 : t2;
        if (t > 0.0f)
        {
            float hit_h = oc_dot_a + t*d_dot_a;
            float half = height*0.5f;
            if (hit_h >= -half && hit_h <= half) return 1; // body hit
        }
    }

    // Top cap
    t_vec3 top_center = { origin.x + axis.x*height*0.5f, origin.y + axis.y*height*0.5f, origin.z + axis.z*height*0.5f };
    float denom = dir.x*axis.x + dir.y*axis.y + dir.z*axis.z;
    if (fabs(denom) > 1e-6f)
    {
        float t_cap = ((top_center.x - ray_origin.x)*axis.x +
                       (top_center.y - ray_origin.y)*axis.y +
                       (top_center.z - ray_origin.z)*axis.z)/denom;
        if (t_cap > 0.0f)
        {
            t_vec3 hit_cap = { ray_origin.x + dir.x*t_cap, ray_origin.y + dir.y*t_cap, ray_origin.z + dir.z*t_cap };
            t_vec3 v = { hit_cap.x-top_center.x, hit_cap.y-top_center.y, hit_cap.z-top_center.z };
            t_vec3 v_perp = { v.x - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.x,
                              v.y - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.y,
                              v.z - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.z };
            float dist2 = v_perp.x*v_perp.x + v_perp.y*v_perp.y + v_perp.z*v_perp.z;
            if (dist2 <= radius*radius) return 1;
        }
    }

    // Bottom cap
    t_vec3 bottom_center = { origin.x - axis.x*height*0.5f, origin.y - axis.y*height*0.5f, origin.z - axis.z*height*0.5f };
    if (fabs(denom) > 1e-6f)
    {
        float t_cap = ((bottom_center.x - ray_origin.x)*axis.x +
                       (bottom_center.y - ray_origin.y)*axis.y +
                       (bottom_center.z - ray_origin.z)*axis.z)/denom;
        if (t_cap > 0.0f)
        {
            t_vec3 hit_cap = { ray_origin.x + dir.x*t_cap, ray_origin.y + dir.y*t_cap, ray_origin.z + dir.z*t_cap };
            t_vec3 v = { hit_cap.x-bottom_center.x, hit_cap.y-bottom_center.y, hit_cap.z-bottom_center.z };
            t_vec3 v_perp = { v.x - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.x,
                              v.y - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.y,
                              v.z - (v.x*axis.x+v.y*axis.y+v.z*axis.z)*axis.z };
            float dist2 = v_perp.x*v_perp.x + v_perp.y*v_perp.y + v_perp.z*v_perp.z;
            if (dist2 <= radius*radius) return 1;
        }
    }

    return 0; // no hit
}
