/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmenegau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/12 18:12:37 by rmenegau          #+#    #+#             */
/*   Updated: 2016/09/17 04:13:44 by rmenegau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LS_H
# define FT_LS_H

# include <sys/stat.h>
# include <dirent.h>
# include <time.h>
# include <pwd.h>
# include <grp.h>
# include "libft.h"

# define STTIME st.st_mtimespec

# define PARAM_A 0x1
# define PARAM_L 0x2
# define PARAM_R 0x4
# define PARAM_T 0x8
# define PARAM_MR 0x10

typedef struct	s_fmt
{
	int	nl;
	int	pw;
	int	gr;
	int	sz;
}				t_fmt;

typedef struct	s_long
{
	int		nl_len;
	char	*user;
	int		user_len;
	char	*grp;
	int		grp_len;
	int		sz_len;
	int		mj_len;
	int		mn_len;
}				t_long;

typedef struct	s_ls
{
	char		*name;
	int			name_len;
	char		*path;
	int			path_len;
	struct stat	st;
	int			type;
	int			nvalid;
	t_long		ldisp;
	struct s_ls	*next;
}				t_ls;

#endif
