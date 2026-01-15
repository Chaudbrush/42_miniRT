/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atolf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 12:28:19 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/15 14:08:02 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minirt.h"

double	ft_atolf(char *str)
{
	int		i;
	int		sign;
	float	power;
	double	num;

	i = 0;
	num = 0;
	sign = 1;
	power = 1;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			sign = -sign;
	while (str[i] >= '0' && str[i] <= '9')
		num = (num * 10) + str[i++] - '0';
	if (str[i] == '.')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		power /= 10;
		num = num + (str[i++] - '0') * power;
	}
	return (num * sign);
}
