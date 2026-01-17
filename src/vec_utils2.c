/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 16:18:34 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/16 07:58:58 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minirt.h"

t_vec3	vec_scale(t_vec3 vector, float s)
{
	return ((t_vec3){vector.x * s, vector.y * s, vector.z * s});
}

float	dot_product(t_vec3 vec1, t_vec3 vec2)
{
	return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

t_vec3	vec_cross(t_vec3 vec1, t_vec3 vec2)
{
	t_vec3	new;

	new.x = vec1.y * vec2.z - vec1.z * vec2.y;
	new.y = vec1.z * vec2.x - vec1.x * vec2.z;
	new.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return (new);
}
