/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lst_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 21:30:03 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/12 14:32:15 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../sphere.h"

void	ft_lstclear(t_types *lst)
{
	t_types	*ptr;

	ptr = lst;
	while (ptr)
	{
		ptr = ptr->next;
		free(lst);
		lst = ptr;
	}
}

void	ft_lstadd_back(t_types **lst, t_types *node)
{
	t_types	*last;

	if (!lst)
		return ;
	if (!*lst)
		*lst = node;
	else
	{
		last = *lst;
		while (last->next)
			last = last->next;
		last->next = node;
	}
}

void	index_list(t_types *lst)
{
	int			i;

	i = 0;
	while (lst)
	{
		((t_sphere *)lst)->index = i;
		lst = lst->next;
		i++;
	}
}

void	correct_string(char *str, char c)
{
	int	len;

	len = ft_strlen(str);
	if (str[len - 1] == c)
		str[len - 1] = ' ';
}

int check_vector(t_vec3 v, char *str)
{
    float len;
    float epsilon = 1e-2;

    len = sqrt(
        v.x*v.x +
        v.y*v.y +
        v.z*v.z
    );

    if (fabs(len - 1.0f) > epsilon)
    {
        print_err(NORMALIZED_ERR, str);
        return (1);
    }
    return (0);
}

// int	check_vector(t_vec3 v, char *str)
// {
// 	t_vec3	normalized;

// 	normalized = normalize_vector(v);
// 	if (v.x != normalized.x || v.y != normalized.y || v.z != normalized.z)
// 	{
// 		print_err(NORMALIZED_ERR, str);
// 		return (1);
// 	}
// 	return (0);
// }
