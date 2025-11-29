/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:24:39 by vloureir          #+#    #+#             */
/*   Updated: 2025/11/29 12:44:51 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPHERE_H
# define SPHERE_H

# include "gnl/get_next_line.h"
# include "minilibx-linux/mlx.h"
# include <fcntl.h>
# include <float.h> // can use FLT_MIN and FLT_MAX, also DBL_MIN and DBL_MAX
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <X11/X.h>
# include <X11/keysym.h>

# define HEIGHT 1000
# define WIDTH 1000
//# define WIDTH (int)(HEIGHT * 1.778)

typedef unsigned char t_uchar;

// TYPES: 'A' (ambient light), 'C' (camera), 'L' (light), 's' (sphere), 'y' (cylinder), 'p' (plane)
typedef struct s_types
{
	int				type;
	struct s_types	*next;
}	t_types;

typedef struct s_sphere
{
	int		type;
	t_types	*next;
	double	coords[3]; // Center of the sphere
	double	radius;
	int		rgb[3]; // Receive the data as 0, 255, 128. Convert it to hex and store in the in the integer
}	t_sphere;

typedef struct s_plane
{
	int		type;
	t_types	*next;
	double	coords[3]; // A point in the plane ?
	int		vector[3]; // Normalized vector, can be from [-1, 1]
	int		rgb[3];
}	t_plane;

typedef struct s_cylinder
{
	int		type;
	t_types	*next;
	double	coords[3]; // Center of the cylinder
	int		vector[3]; // Normalized vector, can be from [-1, 1]
	double	radius;
	double	height;
	int		rgb[3];
}	t_cylinder;

typedef struct s_ambient
{
	float	ratio; // From [0.0, 1.0]
	t_uchar	color[3]; // RGB
}	t_ambient;

typedef struct s_camera
{
	float	coords[3];
	float	norm_vector[3]; // From [-1.0, 1.0]
	int		fov; // From [0, 180]
}	t_camera;

typedef struct s_light
{
	float	coords[3];
	float	brigthness; // From [0.0, 1.0]
	t_uchar	color[3]; // NOT NECESSARY IN MANDATORY
}	t_light;

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
	int		w;
	int		h;
}	t_img;

// DELETE LATER
typedef struct s_circle
{
	int		type;
	double	cords[2]; // Center of the circle
	double	radius;
	int		rgb;
}	t_circle;

typedef struct s_program
{
	void		*mlx;
	void		*win;
	t_img		img_data;
	t_ambient	ambient_light;
	t_camera	camera;
	t_light		light;
	t_types		*objects;
	t_circle	circle;
}	t_program;

int		render(t_program *data);
int		init_mlx(t_program *data);
int		init_data(t_program *data);
int		clean_exit(t_program *data);
int		key_handler(int keysym, t_program *data);
void	my_pixel_put(t_img *img, int x, int y, int color);
int		render_circle(t_program *data, double column, double row);



int		check_extenssion(char *str);
int		not_whitespaces(char *str);
int		ft_strcmp(char *s1, char *s2);
int		parse_line(t_program *data, char *str, t_types **objects);
int		check_args(int argc, char **argv, int *fd);
int		parse_data(t_program *data, int argc, char **argv);


int		get_identifier(char *str);


double	ft_atolf(char *str);
int		ft_atoi(const char *nptr);
void	ft_putstr_fd(char *s, int fd);
char	**ft_split(char const *s, char c);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);



void	free_args(char **argv);
int		is_valid_int(char *str);
int		is_valid_float(char *str, double nb);



int		argv_size(char **argv);
int		init_ambient(t_program *data, char **args);

void	ft_lstclear(t_types *lst);

t_types	*init_plane(char **args);
t_types	*init_sphere(char **args);
t_types	*init_cylinder(char **args);
int		init_light(t_program *data, char **args);
int		init_camera(t_program *data, char **args);
int		init_ambient(t_program *data, char **args);


t_types	*initialize_node(char **args);

void	ft_lstadd_back(t_types **lst, t_types *node);


// DELETE
void	print_list(t_types *lst);


#endif