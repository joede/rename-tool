
/*  main.c - command line mode entry

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#if HAVE_UNISTD_H
  #include <sys/types.h>
  #include <unistd.h>
#endif

#if STDC_HEADERS
  #include <string.h>
#else
  #ifndef HAVE_STRCHR
    #define strchr index
    #define strrchr rindex
  #endif
  char *strchr(), *strrchr();
#endif

#if HAVE_REGEX_H
  #include <regex.h>
#else
  #include "regex.h"
#endif

#include "rename.h"

#define	WHOAMI	"renamex"
#define VERSION	"1.99.1"

static	char	cwd[SVRBUF];	/* current working directory */
static	RENOP	sysopt;

static	char	*usage = "\
Usage: " WHOAMI " [OPTIONS] filename ...\n\
OPTIONS:\n\
  -f, --file              Load file names from the file\n\
  -l, --lowercase         Lowercase the file name\n\
  -u, --uppercase         Uppercase the file name\n\
  -s/PATTERN/STRING[/SW]  Replace the matching PATTERN with STRING.\n\
                          The SW could be:\n\
                          [i] ignore case when searching\n\
                          [b] backward searching and replacing\n\
                          [s] change file's suffix name\n\
                          [r] PATTERN is regular expression\n\
                          [e] PATTERN is extended regular expression\n\
                          [g] replace all occurrences in the filename\n\
                          [1-9] replace specified occurrences in the filename\n\
  -R, --recursive         Operate on files and directories recursively\n"
#ifdef	CFG_UNIX_API
"  -o, --owner OWNER       Change file's ownership (superuser only)\n"
#endif
"  -v, --verbose           Display verbose information\n\
  -t, --test              Test only mode. Do not change any thing\n\
  -h, --help              Display this help and exit\n\
  -V, --version           Output version information and exit\n\
  -A, --always            Always overwrite the existing files\n\
  -N, --never             Never overwrite the existing files\n\
\n\
Please see manpage regex(7) for the details of extended regular expression.\n";

static	char	*version = WHOAMI " " VERSION
", Rename files by substituting the specified patterns.\n\
Copyright (C) 1998-2011 \"Andy Xuming\" <xuming@users.sourceforge.net>\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n";

#ifdef	CFG_UNIX_API
static int cli_set_owner(RENOP *opt, char *optarg);
#endif
static int cli_set_pattern(RENOP *opt, char *optarg);
#ifdef	DEBUG
static int cli_dump(RENOP *opt, char *filename);
#endif
static void siegfried (int signum);

int main(int argc, char **argv)
{
	struct	sigaction	signew, sigold;
	int 	infile = 0, rc = RNM_ERR_NONE;

	memset(&sysopt, 0, sizeof(RENOP));
	sysopt.compare = strncmp;
	while (--argc && (**++argv == '-')) {
		rc = RNM_ERR_NONE;
		if (!strcmp_list(*argv, "-h", "--help")) {
			puts(usage);
			rc = RNM_ERR_HELP;
		} else if (!strcmp_list(*argv, "-V", "--version")) {
			puts(version);
			rc = RNM_ERR_HELP;
		} else if (!strcmp_list(*argv, "-f", "--file")) {
			infile = 1;
		} else if (!strcmp_list(*argv, "-l", "--lowercase")) {
			sysopt.oflags &= ~RNM_OFLAG_MASKCASE;
			sysopt.oflags |= RNM_OFLAG_LOWERCASE;
		} else if (!strcmp_list(*argv, "-u", "--uppercase")) {
			sysopt.oflags &= ~RNM_OFLAG_MASKCASE;
			sysopt.oflags |= RNM_OFLAG_UPPERCASE;
		} else if (!strcmp_list(*argv, "-R", "--recursive")) {
			sysopt.cflags |= RNM_CFLAG_RECUR;
		} else if (!strcmp_list(*argv, "-v", "--verbose")) {
			sysopt.cflags |= RNM_CFLAG_VERBOSE;
		} else if (!strcmp_list(*argv, "-t", "--test-only")) {
			sysopt.cflags |= RNM_CFLAG_TEST | RNM_CFLAG_VERBOSE;
		} else if (!strcmp_list(*argv, "-A", "--always")) {
			sysopt.cflags &= ~RNM_CFLAG_PROMPT_MASK;
			sysopt.cflags |= RNM_CFLAG_ALWAYS;
		} else if (!strcmp_list(*argv, "-N", "--never")) {
			sysopt.cflags &= ~RNM_CFLAG_PROMPT_MASK;
			sysopt.cflags |= RNM_CFLAG_NEVER;
#ifdef	CFG_UNIX_API
		} else if (!strcmp_list(*argv, "-o", "--owner")) {
			if (--argc == 0) {
				rc = RNM_ERR_PARAM;
			} else {
				rc = cli_set_owner(&sysopt, *++argv);
			}
#endif
		} else if (argv[0][1] == 's') {
			if (argv[0][2] != 0) {
				rc = cli_set_pattern(&sysopt, argv[0]+2);
			} else if (--argc == 0) {
				rc = RNM_ERR_PARAM;
			} else {
				rc = cli_set_pattern(&sysopt, *++argv);
			}
		} else {
			printf("Unknown option. [%s]\n", *argv);
			rc = RNM_ERR_PARAM;
		}
		if (rc != RNM_ERR_NONE) {
			return rc;
		}
	}

	if ((argc < 1) || (!sysopt.oflags && !sysopt.action)) {
		puts(usage);
		return RNM_ERR_HELP;
	}
	
	if (getcwd(cwd, SVRBUF) == NULL)  {
		printf("Out of path!\n");
		return RNM_ERR_GETDIR;
	}
    
	signew.sa_handler = siegfried;
	sigemptyset(&signew.sa_mask);
	signew.sa_flags = 0;
	
	sigaction(SIGINT, NULL, &sigold);
	if (sigold.sa_handler != SIG_IGN) {
		sigaction(SIGINT, &signew, NULL);
	}
	sigaction(SIGHUP, NULL, &sigold);
	if (sigold.sa_handler != SIG_IGN) {
		sigaction(SIGHUP, &signew, NULL);
	}
	sigaction(SIGTERM, NULL, &sigold);
	if (sigold.sa_handler != SIG_IGN) {
		sigaction(SIGTERM, &signew, NULL);
	}

#ifdef	DEBUG
	if (sysopt.cflags & RNM_CFLAG_TEST) {
		cli_dump(&sysopt, *argv);
	}
#endif
	while (argc-- && (rc == RNM_ERR_NONE))  {
		if (infile) {
			rc = rename_enfile(&sysopt, *argv++);
		} else {
			rc = rename_entry(&sysopt, *argv++);
		}
	}

	if (sysopt.action == RNM_ACT_REGEX) { 
		regfree(sysopt.preg);
	}
	printf("%d files renamed.\n", sysopt.rpcnt);
	return rc;
}

#ifdef	CFG_UNIX_API
static int cli_set_owner(RENOP *opt, char *optarg)
{
	struct	passwd	pwd, *result;
	char	*buf;
	size_t	bufsize;

	if (geteuid() != 0) {
		printf("Access denied!\n");
		return RNM_ERR_EUID;
	}

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1) {		/* Value was indeterminate */
		bufsize = 16384;	/* Should be more than enough */
	}
	if ((buf = malloc(bufsize)) == NULL) {
		return RNM_ERR_LOWMEM;
	}
	if (getpwnam_r(optarg, &pwd, buf, bufsize, &result)) {
		printf("Access failed!\n");
		free(buf);
		return RNM_ERR_EUID;
	}
	if (result == NULL) {
		printf("User not existed!\n");
		free(buf);
		return RNM_ERR_EUID;
	}

	opt->pw_uid = pwd.pw_uid;
	opt->pw_gid = pwd.pw_gid;
	opt->oflags |= RNM_OFLAG_OWNER;
	free(buf);
	return RNM_ERR_NONE;
}
#endif	/* CFG_UNIX_API */

static int cli_set_pattern(RENOP *opt, char *optarg)
{
	char	*idx[4], *p; 
	int	cflags = 0;

	/* skip the first separater */
	if ((*optarg == '/') || (*optarg == ':')) {
		optarg++;
	}
	
	fixtoken(optarg, idx, 4, "/:");
    	opt->pattern = idx[0];
	opt->substit = idx[1];

	if (!opt->pattern || !opt->substit) {
		return RNM_ERR_PARAM;
	}

	opt->pa_len = strlen(opt->pattern);
	opt->su_len = strlen(opt->substit);
	opt->action = RNM_ACT_FORWARD;
	opt->count = 1;		/* default replace once */
    	for (p = idx[2]; p && *p; p++)  {
		switch (*p)  {
		case 'g':
		case 'G':
			opt->count = 0;	/* 0 = unlimit */
			break;
		case 'b':
		case 'B':
			opt->action = RNM_ACT_BACKWARD;
			break;
		case 's':
		case 'S':
	    		opt->action = RNM_ACT_SUFFIX;
	    		break;
		case 'i':
		case 'I':
			cflags |= REG_ICASE;
			opt->compare = strncasecmp;
			break;
		case 'r':
		case 'R':
			opt->action = RNM_ACT_REGEX;
			break;
		case 'e':
		case 'E':
	    		opt->action = RNM_ACT_REGEX;
			cflags |= REG_EXTENDED;
			break;
		default:
			if (isdigit((int) *p)) {
				opt->count = *p - '0';
			}
			break;
		}
	}
	if (opt->action == RNM_ACT_REGEX) {
		if (regcomp(opt->preg, opt->pattern, cflags))  {
			printf("Wrong regular expression. [%s]\n", 
					opt->pattern);
			return RNM_ERR_REGPAT;
		}
	}
	return RNM_ERR_NONE;
}

#ifdef	DEBUG
static int cli_dump(RENOP *opt, char *filename)
{
	printf("Source:         %s\n", filename);
	printf("Flags:          OF=%x CF=%x ACT=%d\n", 
			opt->oflags, opt->cflags, opt->action);
	printf("Owership:       UID=%d GID=%d\n",
			(int)opt->pw_uid, (int)opt->pw_gid);
	printf("Pattern:        %s (%d)\n", opt->pattern, opt->pa_len);
	printf("Substituter:    %s (%d+%d)\n", 
			opt->substit, opt->su_len, opt->count);
	printf("Name Buffer:    %d (%d)\n", opt->room, FNBUF);
	printf("\n");
	return 0;
}
#endif	/* DEBUG */


/* Zis is KAOS! */
static void siegfried (int signum)
{
	if (sysopt.action == RNM_ACT_REGEX) {
		regfree(sysopt.preg);
	}
	if (chdir(cwd) < 0) {	/* kill the warning */
		perror("chdir");
	}
	exit(signum);
}



