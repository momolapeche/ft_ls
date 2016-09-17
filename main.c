/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmenegau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/12 18:11:04 by rmenegau          #+#    #+#             */
/*   Updated: 2016/09/17 04:21:54 by rmenegau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"

int		nb_digits(int n)
{
	int	i;

	i = 1;
	while ((n /= 10))
		i++;
	return (i);
}

int		get_mj(dev_t n)
{
	return (n % 256);
}

int		get_mn(dev_t n)
{
	while (n > 256)
		n /= 256;
	return (n);
}

void	get_long(t_ls *l)
{
	void	*tmp;

	l->ldisp.nl_len = nb_digits(l->st.st_nlink);
	tmp = getpwuid(l->st.st_uid);
	l->ldisp.user = ft_strdup(((struct passwd *)tmp)->pw_name);
	l->ldisp.user_len = ft_strlen(((struct passwd *)tmp)->pw_name);
	tmp = getgrgid(l->st.st_gid);
	l->ldisp.grp = ft_strdup(((struct group *)tmp)->gr_name);
	l->ldisp.grp_len = ft_strlen(((struct group *)tmp)->gr_name);
	l->ldisp.sz_len = nb_digits(l->st.st_size);
	if (S_ISBLK(l->st.st_mode) || S_ISBLK(l->st.st_mode))
	{
		l->ldisp.mj_len = nb_digits(get_mj(l->st.st_rdev));
		l->ldisp.mn_len = nb_digits(get_mn(l->st.st_rdev));
	}
	else
	{
		l->ldisp.mj_len = 0;
		l->ldisp.mn_len = 0;
	}
}

t_ls	*new_path(char *path, char *name, int p)
{
	t_ls	*ret;

	ret = (t_ls *)malloc(sizeof(t_ls));
	ret->path = path;
	ret->path_len = ft_strlen(path);
	ret->name = name;
	ret->name_len = ft_strlen(name);
	if (!(ret->nvalid = lstat(path, &ret->st) && stat(path, &ret->st))
			&& p & PARAM_L)
		get_long(ret);
	ret->type = ret->nvalid || S_ISDIR(ret->st.st_mode);
	ret->next = NULL;
	return (ret);
}

void	add_path_time(t_ls **b, t_ls *n)
{
	t_ls	*l;

	if (!*b)
		*b = n;
	else if ((*b)->STTIME.tv_sec < n->STTIME.tv_sec ||
			((*b)->STTIME.tv_sec == n->STTIME.tv_sec &&
			((*b)->STTIME.tv_nsec < n->STTIME.tv_nsec ||
			((*b)->STTIME.tv_nsec == n->STTIME.tv_nsec &&
			ft_strcmp((*b)->name, n->name) > 0))))
	{
		n->next = *b;
		*b = n;
	}
	else
	{
		l = *b;
		while (l->next && (l->next->STTIME.tv_sec > n->STTIME.tv_sec ||
				(l->next->STTIME.tv_sec == n->STTIME.tv_sec &&
				(l->next->STTIME.tv_nsec > n->STTIME.tv_nsec ||
				(l->next->STTIME.tv_nsec == n->STTIME.tv_nsec &&
				ft_strcmp(l->next->name, n->name) < 0)))))
			l = l->next;
		n->next = l->next;
		l->next = n;
	}
}

void	add_path_ascii(t_ls **b, t_ls *n)
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

void	add_path(t_ls **b, t_ls *n, int p)
{
	if (p & PARAM_T)
		add_path_time(b, n);
	else
		add_path_ascii(b, n);
}

void	illegal_option(char c)
{
	write(1, "./ft_ls: illegal option -- ", 27);
	write(1, &c, 1);
	write(1, "\n", 1);
	write(1, "usage: ./ft_ls [-Ralrt] [file ...]\n", 35);
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

t_ls	*rev_ls(t_ls *l)
{
	t_ls	*new;
	t_ls	*next;

	new = NULL;
	while (l)
	{
		next = l->next;
		l->next = new;
		new = l;
		l = next;
	}
	return (new);
}

void	get_fmt(t_ls *l, t_fmt *fmt)
{
	int	tmp;

	while (l)
	{
		if ((tmp = nb_digits(l->st.st_nlink)) > fmt->nl)
			fmt->nl = tmp;
		if (l->ldisp.grp_len > fmt->gr)
			fmt->gr = l->ldisp.grp_len;
		if (l->ldisp.user_len > fmt->pw)
			fmt->pw = l->ldisp.user_len;
		if ((tmp = nb_digits(l->st.st_size)) > fmt->sz)
			fmt->sz = tmp;
		l = l->next;
	}
	fmt->nl++;
	fmt->gr++;
	fmt->sz++;
	fmt->pw++;
	fmt->pw++;
}

t_ls	*get_paths(int ac, char **av, int *p, t_fmt *fmt)
{
	int			i;
	static t_ls	*ls[4];

	i = get_options(ac, av, p);
	ft_bzero(ls, sizeof(t_ls *) * 4);
	while (i < ac)
	{
		if ((ls[3] = new_path(ft_strdup(av[i]), ft_strdup(av[i]), *p)))
		{
			if (ls[3]->nvalid)
				add_path(&ls[0], ls[3], 0);
			else if (ls[3]->type)
				add_path(&ls[2], ls[3], *p);
			else
				add_path(&ls[1], ls[3], *p);
		}
		i++;
	}
	if (*p & PARAM_R)
	{
		ls[1] = rev_ls(ls[1]);
		ls[2] = rev_ls(ls[2]);
	}
	get_fmt(ls[1], fmt);
	return (paths_cat(ls[0], ls[1], ls[2]));
}

void	free_ls(t_ls *paths, int p)
{
	t_ls	*l;

	while (paths)
	{
		free(paths->name);
		free(paths->path);
		if (p & PARAM_L)
		{
			free(paths->ldisp.user);
			free(paths->ldisp.grp);
		}
		l = paths;
		paths = paths->next;
		free(l);
	}
}

char	*path_join(char *path, char *name)
{
	char	*r;

	r = (char *)malloc(sizeof(char) * (ft_strlen(path) + ft_strlen(name) + 2));
	ft_strcpy(r, path);
	ft_strcat(r, "/");
	ft_strcat(r, name);
	return (r);
}

void	print_attr(mode_t m)
{
	static char	t[11];

	if (S_ISREG(m))
		t[0] = '-';
	else if (S_ISBLK(m) | S_ISCHR(m))
		t[0] = S_ISBLK(m) ? 'b' : 'c';
	else if (S_ISDIR(m))
		t[0] = 'd';
	else if (S_ISLNK(m) | S_ISFIFO(m))
		t[0] = S_ISLNK(m) ? 'l' : 'p';
	else if (S_ISSOCK(m) | S_ISWHT(m))
		t[0] = S_ISSOCK(m) ? 's' : 'w';
	else
		t[0] = ' ';
	t[1] = m & 0400 ? 'r' : '-';
	t[2] = m & 0200 ? 'w' : '-';
	t[3] = m & 0100 ? 'x' : '-';
	t[4] = m & 0040 ? 'r' : '-';
	t[5] = m & 0020 ? 'w' : '-';
	t[6] = m & 0010 ? 'x' : '-';
	t[7] = m & 0004 ? 'r' : '-';
	t[8] = m & 0002 ? 'w' : '-';
	t[9] = m & 0001 ? 'x' : '-';
	t[10] = ' ';
	write(1, t, 11);
}

void	print_spaces(int len)
{
	static char	*s = NULL;
	static int	s_len;

	if (len == -1)
	{
		if (s)
			free(s);
		return ;
	}
	if (!s || len > s_len)
	{
		if (s)
			free(s);
		s = (char *)malloc(sizeof(char) * (len + 1));
		ft_memset(s, ' ', len);
		s_len = len;
		s[len] = '\0';
	}
	write(1, s, len);
}

void	long_disp(t_ls *l, t_fmt *fmt)
{
	static time_t	curr = 0;
	static char		lnk[1024];
	int				lnk_len;
	char			*tmp;

	if (!curr)
		curr = time(NULL);
	print_attr(l->st.st_mode);
	print_spaces(fmt->nl - l->ldisp.nl_len);
	ft_putnbr(l->st.st_nlink);
	write(1, " ", 1);
	write(1, l->ldisp.user, l->ldisp.user_len);
	print_spaces(fmt->pw - l->ldisp.user_len);
	write(1, l->ldisp.grp, l->ldisp.grp_len);
	print_spaces(fmt->gr - l->ldisp.grp_len);
	print_spaces(fmt->sz - l->ldisp.sz_len);
	ft_putnbr(l->st.st_size);
	write(1, " ", 1);
	tmp = ctime(&l->st.st_mtimespec.tv_sec);
	if (l->st.st_mtimespec.tv_sec < curr - 15778800)
		ft_memcpy(&tmp[11], &tmp[19], 5);
	write(1, &tmp[4], 12);
	write(1, " ", 1);
	write(1, l->name, l->name_len);
	if (S_ISLNK(l->st.st_mode))
	{
		write(1, " -> ", 4);
		lnk_len = readlink(l->path, lnk, 1024);
		write(1, lnk, lnk_len);
	}
}

t_ls	*aff_wrongs_and_files(t_ls *paths, int p, t_fmt *fmt)
{
	while (paths && paths->nvalid)
	{
		write(1, "./ft_ls: ", 9);
		write(1, paths->path, paths->path_len);
		write(1, ": No such file or directory\n", 28);
		paths = paths->next;
	}
	if (paths && paths->type)
		return (paths);
	while (paths && !paths->type)
	{
		if (p & PARAM_L)
			long_disp(paths, fmt);
		else
			write(1, paths->path, paths->path_len);
		write(1, "\n", 1);
		paths = paths->next;
	}
	if (paths)
		write(1, "\n", 1);
	return (paths);
}

void	ft_ls(char *path, int p, t_fmt *fmt)
{
	DIR				*dir;
	struct dirent	*f;
	t_ls			*content;
	t_ls			*l;

	content = NULL;
	ft_bzero(fmt, sizeof(t_fmt));
	if ((dir = opendir(path)))
	{
		while ((f = readdir(dir)))
		{
			if (PARAM_A & p || f->d_name[0] != '.')
				add_path(&content, new_path(path_join(path, f->d_name), ft_strdup(f->d_name), p), p);
		}
		get_fmt(content, fmt);
		closedir(dir);
		if (p & PARAM_R)
			content = rev_ls(content);
		l = content;
		while (l)
		{
			if (p & PARAM_A || (l->name && l->name[0] != '.'))
			{
				if (p & PARAM_L)
					long_disp(l, fmt);
				else
					write(1, l->name, l->name_len);
				write(1, "\n", 1);
			}
			l = l->next;
		}
		l = content;
		while (l)
		{
			if (l->type && p & PARAM_MR &&
					(l->name[0] != '.' || (p & PARAM_A &&
					!ft_strequ(l->name, ".") && !ft_strequ(l->name, ".."))))
			{
				write(1, "\n", 1);
				write(1, l->path, l->path_len);
				write(1, ":\n", 2);
				ft_ls(l->path, p, fmt);
			}
			l = l->next;
		}
	}
	free_ls(content, p);
}

int		main(int ac, char **av)
{
	t_ls	*paths;
	t_ls	*l;
	int		p;
	t_fmt	fmt;

	ft_bzero(&fmt, sizeof(t_fmt));
	if ((paths = get_paths(ac, av, &p, &fmt)) == NULL)
		paths = new_path(ft_strdup("."), ft_strdup("."), p);
	l = aff_wrongs_and_files(paths, p, &fmt);
	while (l)
	{
		if (l != paths || l->next)
		{
			write(1, l->path, l->path_len);
			write(1, ":\n", 2);
		}
		ft_ls(l->path, p, &fmt);
		l = l->next;
		if (l)
			write(1, "\n", 1);
	}
//	sleep(5);
	free_ls(paths, p);
	print_spaces(-1);
	return (0);
}
