/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 10:24:39 by vloureir          #+#    #+#             */
/*   Updated: 2026/01/08 12:29:02 by vloureir         ###   ########.fr       */
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
# define MY_PI 3.14159265

# define FILE_ERR "-error: Missing file argument\n"
# define ARGV_ERR "-error: Too many arguments\n"
# define EXTENSION_ERR "-error: Wrong extension\n"
# define EMPTY_ERR "-error: Empty file\n"
# define MALLOC_ERR "-error: Malloc failure\n"
# define QUANTITY_ERR "-error: Invalid amount of arguments: "
# define NUMBER_ERR "-error: Invalid numeric arguments: "
# define INVALID_ERR "-error: Invalid identifier: "
# define NORMALIZED_ERR "-error: Vector not normalized: "

typedef unsigned char t_uchar;

typedef struct s_vec3
{
	float	x;
	float	y;
	float	z;
}	t_vec3;

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
	int		index;
	t_vec3	coords;
	float	radius;
	t_vec3	color;
}	t_sphere;

typedef struct s_plane
{
	int		type;
	t_types	*next;
	int		index;
	t_vec3	coords;
	t_vec3	vector;
	t_vec3	color;
}	t_plane;

typedef struct s_cylinder
{
	int		type;
	t_types	*next;
	int		index;
	t_vec3	coords;
	t_vec3	vector;
	float	radius;
	float	height;
	t_vec3	color;
}	t_cylinder;

typedef struct s_ambient
{
	float	ratio;
	t_vec3	color;
}	t_ambient;

typedef struct s_camera
{
	t_vec3	coords;
	t_vec3	vector;
	int		fov;
}	t_camera;

typedef struct s_light
{
	t_vec3	coords;
	float	brigthness;
	t_vec3	color;
}	t_light;

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
}	t_img;

typedef struct s_program
{
	void		*mlx;
	void		*win;
	t_img		img_data;
	t_ambient	ambient;
	t_camera	camera;
	t_light		light;
	t_types		*objects;
	float		hitpoint;
}	t_program;

// Parse
int		is_valid_int(char *str);
int		is_valid_float(char *str, double nb);
int		check_args(int argc, char **argv, int *fd);
int		parse_data(t_program *data, int argc, char **argv, int count);
int		parse_line(t_program *data, char *str, t_types **objects, int flag);



// Helper Functions
void	free_args(char **argv);
int		argv_size(char **argv);
int		not_whitespaces(char *str);
int		check_extenssion(char *str);
int		ft_strcmp(char *s1, char *s2);

// Init Data
int		init_light(t_program *data, char **args);
int		init_camera(t_program *data, char **args);
int		init_ambient(t_program *data, char **args);
int		init_int_array(char *str, t_vec3 *color, char *type);
int		init_float_array(char *str, t_vec3 *vector, int flag, char *type);

// Init Nodes
t_types	*init_plane(char **args);
t_types	*init_sphere(char **args);
t_types	*init_cylinder(char **args);
t_types	*initialize_node(char **args);

// Lst Utils
void	index_list(t_types *lst);
void	ft_lstclear(t_types *lst);
void	correct_string(char *str, char c);
int		check_vector(t_vec3 v, char *str);
void	ft_lstadd_back(t_types **lst, t_types *node);





int		check_shadow(t_program *data, t_vec3 ray_origin, t_vec3 ray_dir, int i);
int 	shadow_plane(t_vec3 dir, t_plane *plane, t_vec3 light, t_program *data, t_vec3 ray_origin);
int		shadow_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, t_program *data, t_vec3 ray_origin);
int		shadow_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light, t_program *data, t_vec3 ray_origin);



// NOT ORGANIZED

int		render(t_program *data);
int		init_mlx(t_program *data);
int		clean_exit(t_program *data);
int		key_handler(int keysym, t_program *data);
void	my_pixel_put(t_img *img, int x, int y, int color);
int		render_circle(t_program *data, double column, double row);



double	ft_atolf(char *str);
int		ft_atoi(const char *nptr);
void	ft_putstr_fd(char *s, int fd);
char	**ft_split(char const *s, char c);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);



int to_rgb(t_vec3 c);

int	rt(t_program *data, t_vec3 vec);
int raytrace_plane(t_vec3 dir, t_plane *plane, t_vec3 light, t_program *data);
int raytrace_sphere(t_vec3 dir, t_sphere *sphere, t_vec3 light, t_program *data);
int raytrace_cylinder(t_vec3 dir, t_cylinder *cyl, t_vec3 light, t_program *data);

// Vector Functions
t_vec3	normalize_vector(t_vec3 vector);
t_vec3	vec_add(t_vec3 vec1, t_vec3 vec2);
t_vec3	vec_sub(t_vec3 vec1, t_vec3 vec2);
t_vec3	vec_scale(t_vec3 vector, float s);
t_vec3	vec_mult(t_vec3 vec1, t_vec3 vec2);
float	dot_product(t_vec3 v1, t_vec3 v2);

int	phong_color(t_program *data, t_vec3 color, t_vec3 normal, t_vec3 hit, t_vec3 light);


// DELETE
void	print_list(t_types *lst);

void	print_err(char *s1, char *s2);

#endif