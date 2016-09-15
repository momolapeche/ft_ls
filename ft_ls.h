/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmenegau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/12 18:12:37 by rmenegau          #+#    #+#             */
/*   Updated: 2016/09/12 21:49:03 by rmenegau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LS_H
# define FT_LS_H

# include <sys/stat.h>
# include <dirent.h>
# include "libft.h"

# define PARAM_A 0x1
# define PARAM_L 0x2
# define PARAM_R 0x4
# define PARAM_T 0x8
# define PARAM_MR 0x10

typedef struct	s_ls
{
	char		*name;
	int			name_len;
	char		*path;
	int			path_len;
	struct stat	st;
	int			type;
	int			nvalid;
	struct s_ls	*next;
}				t_ls;

#endif
