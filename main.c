/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmenegau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/12 18:11:04 by rmenegau          #+#    #+#             */
/*   Updated: 2016/09/15 13:55:39 by rmenegau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"

t_ls	*new_path(char *path, char *name)
{
	t_ls	*ret;

	ret = (t_ls *)malloc(sizeof(t_ls));
	ret->path = path;
	ret->path_len = ft_strlen(path);
	ret->nvalid = lstat(path, &ret->st) && stat(path, &ret->st);
	ret->name = name;
	ret->name_len = ft_strlen(name);
	ret->type = ret->nvalid || S_ISDIR(ret->st.st_mode);
	ret->next = NULL;
	return (ret);
}

void	add_path(t_ls **b, t_ls *n)
{
	t_ls	*l;

	if (!*b)
		*b = n;
	else if (ft_strcmp((*b)->name, n->name) > 0)
	{
		n->next = *b;
		*b = n;
	}
	else
	{
		l = *b;
		while (l->next && ft_strcmp(l->next->name, n->name) < 0)
			l = l->next;
		n->next = l->next;
		l->next = n;
	}
}

void	illegal_option(char c)
{
	ft_printf("./ft_ls: illegal option -- %c\n", c);
	ft_putstr("usage: ./ft_ls [-Ralrt] [file ...]\n");
	exit(0);
}

int		is_option(char c)
{
	if (c == 'a')
		return (PARAM_A);
	if (c == 'l')
		return (PARAM_L);
	if (c == 'r')
		return (PARAM_R);
	if (c == 't')
		return (PARAM_T);
	if (c == 'R')
		return (PARAM_MR);
	illegal_option(c);
	return (0);
}

int		get_options(int ac, char **av, int *p)
{
	int	i;
	int	j;

	*p = 0;
	i = 1;
	while (i < ac && av[i][0] == '-' && av[i][1])
	{
		j = 1;
		while (j == 1 || av[i][j])
		{
			*p |= is_option(av[i][j]);
			j++;
		}
		i++;
	}
	return (i);
}

t_ls	*paths_cat(t_ls *wrongs, t_ls *files, t_ls *dirs)
{
	t_ls	*tmp;

	if (wrongs)
	{
		tmp = wrongs;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = files;
	}
	else
		wrongs = files;
	if (wrongs)
	{
		tmp = wrongs;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = dirs;
		return (wrongs);
	}
	else
		return (dirs);
}

t_ls	*get_paths(int ac, char **av, int *p)
{
	int		i;
	t_ls	*wrongs;
	t_ls	*dirs;
	t_ls	*files;
	t_ls	*tmp;

	i = get_options(ac, av, p);
	wrongs = NULL;
	dirs = NULL;
	files = NULL;
	while (i < ac)
	{
		if ((tmp = new_path(ft_strdup(av[i]), ft_strdup(av[i]))))
		{
			if (tmp->nvalid)
				add_path(&wrongs, tmp);
			else if (tmp->type)
				add_path(&dirs, tmp);
			else
				add_path(&files, tmp);
		}
		i++;
	}
	return (paths_cat(wrongs, files, dirs));
}

t_ls	*aff_wrongs_and_files(t_ls *paths)
{
	while (paths && paths->nvalid)
	{
		ft_printf("./ft_ls: %s: No such file or directory\n", paths->path);
		paths = paths->next;
	}
	if (paths && paths->type)
		return (paths);
	while (paths && !paths->type)
	{
		ft_printf("%s\n", paths->path);
		paths = paths->next;
	}
	if (paths)
		write(1, "\n", 1);
	return (paths);
}

void	free_ls(t_ls *paths)
{
	t_ls	*l;

	while (paths)
	{
		if (paths->name)
			free(paths->name);
		free(paths->path);
		l = paths;
		paths = paths->next;
		l->next = NULL;
		free(l);
	}
}

char	*path_join(char *path, char *name)
{
	char	*ret;

	ret = (char *)malloc(sizeof(char) * (ft_strlen(path) + ft_strlen(name) + 2));
	ft_strcpy(ret, path);
	ft_strcat(ret, "/");
	ft_strcat(ret, name);
	return (ret);
}

void	ft_ls(char *path, int p)
{
	DIR				*dir;
	struct dirent	*file;
	t_ls			*content;
	t_ls			*l;

	content = NULL;
	if ((dir = opendir(path)))
	{
		while ((file = readdir(dir)))
			add_path(&content, new_path(path_join(path, file->d_name), ft_strdup(file->d_name)));
		closedir(dir);
		l = content;
//		ft_printf("((%s))\n", l->path);
//		write(1, "lolipop0\n", 9);
		while (l)
		{
			if (p & PARAM_A || (l->name && l->name[0] != '.'))
				ft_printf("%s\n", l->name);
//			printf("%p\n", l);
			l = l->next;
		}
//		write(1, "lolipop1\n", 9);
		l = content;
		while (l)
		{
			if (l->type && p & PARAM_MR &&
					(l->name[0] != '.' || (p & PARAM_A &&
					!ft_strequ(l->name, ".") && !ft_strequ(l->name, ".."))))
			{
				write(1, "\n", 1);
				write(1, l->path, l->path_len);
				write(1, "\n", 1);
//				ft_printf("\n%s:\n", l->path);
				ft_ls(l->path, p);
			}
			l = l->next;
		}
	}
	free_ls(content);
}

int		main(int ac, char **av)
{
	t_ls	*paths;
	t_ls	*l;
	int		p;

	if ((paths = get_paths(ac, av, &p)) == NULL)
		paths = new_path(ft_strdup("."), ft_strdup("."));
	l = aff_wrongs_and_files(paths);
	while (l)
	{
		if (l != paths || paths->next)
		{
			write(1, l->path, l->path_len);
			write(1, ":\n", 2);
		}
		ft_ls(l->path, p);
		l = l->next;
		if (l)
			write(1, "\n", 1);
	}
	free_ls(paths);
	paths = NULL;
	sleep(5);
	return (0);
}
