/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vloureir <vloureir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:22:05 by vloureir          #+#    #+#             */
/*   Updated: 2025/05/17 15:41:44 by vloureir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

char	*ft_strjoin(char *s1, char const *s2)
{
	int		i;
	int		j;
	size_t	len;
	char	*new;

	i = 0;
	j = 0;
	len = ft_strlen(s1) + ft_strlen(s2);
	new = malloc(sizeof(char) * (len + 1));
	if (!new)
		return (free(s1), NULL);
	while (s1 && s1[i])
		new[j++] = s1[i++];
	i = 0;
	while (s2[i])
		new[j++] = s2[i++];
	new[j] = '\0';
	free(s1);
	return (new);
}

void	ft_read_and_store(int fd, char **line, char *buffer)
{
	int		bytes_read;

	bytes_read = 0;
	if (buffer[0])
		*line = ft_strjoin(*line, buffer);
	while (!check_newline(buffer))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 1)
			break ;
		buffer[bytes_read] = '\0';
		*line = ft_strjoin(*line, buffer);
	}
	if (bytes_read == -1)
	{
		free(*line);
		*line = NULL;
		buffer[0] = '\0';
	}
}

void	clean_buffer(char *buffer)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	while (buffer[i])
		buffer[j++] = buffer[i++];
	buffer[j] = '\0';
}

int	check_newline(char *buffer)
{
	int	i;

	i = -1;
	while (buffer[++i])
	{
		if (buffer[i] == '\n')
			return (1);
	}
	return (0);
}
