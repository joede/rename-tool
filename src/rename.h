
/* 
    Copyright (C) 1998-2011  "Andy Xuming" <xuming@users.sourceforge.net>

    This file is part of RENAME, a utility to help file renaming

    RENAME is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RENAME is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef	_RENAME_H_
#define _RENAME_H_
  
#define RNM_ERR_NONE		0
#define RNM_ERR_SKIP		1
#define RNM_ERR_HELP		-1
#define RNM_ERR_PARAM		-2	/* wrong (command line) parameters */
#define RNM_ERR_GETDIR		-3	/* can not get directory name */
#define RNM_ERR_EUID		-4	/* invalid user id */
#define RNM_ERR_LOWMEM		-5	/* low memory */
#define RNM_ERR_REGPAT		-6	/* wrong regex pattern */
#define RNM_ERR_STAT		-7	/* failed to stat of the file */
#define RNM_ERR_CHDIR		-8
#define RNM_ERR_OPENDIR		-9
#define RNM_ERR_LONGPATH	-10	/* path name is too long */
#define RNM_ERR_OPENFILE	-11
#define RNM_ERR_OVERFLOW	-12
#define RNM_ERR_RENAME		-13
#define RNM_ERR_CHOWN		-14


#define RNM_CFLAG_NONE		0
#define RNM_CFLAG_NEVER		1	/* say no to all existed files */
#define RNM_CFLAG_ALWAYS 	2	/* say yes to all existed files */
#define RNM_CFLAG_PROMPT_MASK	3
#define RNM_CFLAG_RECUR		0x100	/* recursive operation */
#define RNM_CFLAG_VERBOSE	0x200	/* verbose mode */
#define RNM_CFLAG_TEST		0x400	/* test mode only */

#define	RNM_OFLAG_NONE		0	/* do not change output filename */
#define RNM_OFLAG_LOWERCASE	1	/* lowercase the output filename */
#define RNM_OFLAG_UPPERCASE	2	/* uppercase the output filename */
#define RNM_OFLAG_MASKCASE	3
#define RNM_OFLAG_OWNER		4	/* change the file's ownership */

#define RNM_ACT_NONE		0
#define RNM_ACT_FORWARD		1	/* search and substitute simplely */
#define RNM_ACT_BACKWARD	2	/* search and substitute backwardly */
#define RNM_ACT_REGEX		3	/* enable regular expression */
#define	RNM_ACT_SUFFIX		6	/* append the suffix */

#define RNM_REP_OK		0
#define RNM_REP_SKIP		1
#define RNM_REP_TEST		2
#define RNM_REP_FAILED		3
#define RNM_REP_CHOWN		4

#define	SVRBUF	512
#define FNBUF	4096

typedef	struct	{
	int	oflags;
	int	cflags;
	int	action;

	uid_t	pw_uid;
	gid_t	pw_gid;

	char	*pattern;
	int	pa_len;
	char	*substit;
	int	su_len;
	int	count;		/* replace occurance */
	regex_t	preg[1];

	int	(*compare)(const char *s1, const char *s2, size_t n);

	char	buffer[FNBUF];		/* hope that's big enough */
	int	room;
	int	rpcnt;
} RENOP;


#define strcmp_list(dst,s1,s2)	(strcmp((dst),(s1)) && strcmp((dst),(s2)))

int rename_enfile(RENOP *opt, char *filename);
int rename_entry(RENOP *opt, char *filename);

int safe_copy(char *dest, const char *src, size_t n);
int safe_cat(char *dest, const char *src, size_t n);
char *skip_space(char *sour);

/* see fixtoken.c */

int fixtoken(char *sour, char **idx, int ids, char *delim);
int ziptoken(char *sour, char **idx, int ids, char *delim);
  
#endif

