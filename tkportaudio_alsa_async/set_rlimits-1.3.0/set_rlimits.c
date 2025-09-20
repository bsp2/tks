/*
 * A quick and dirty program to allow setting of the realtime resource
 * limits under kernel 2.6.12 or later on systems which don't use PAM.
 *
 * Copyright (C) 2005-2006, 2008 Jonathan Woithe
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *       
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *       
 */
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

/* ======================================================================== */

/* Define extended RT limit values if these are for some reason
 * not visible (from taken 2.6.12).
 */
#ifndef RLIMIT_NICE
#define RLIMIT_NICE            13      /* max nice prio allowed to raise to
                                          0-39 for nice level 19 .. -20 */
#define RLIMIT_RTPRIO          14      /* maximum realtime priority */
#endif

/* ======================================================================== */

/* Globals for configuration purposes */

#define VERSION "1.3.0"

/* Location and name of the configuration file.  Normally this will be 
 * something like /etc/set_rlimits.conf.
 */
#define CONFIG_FILE "/etc/set_rlimits.conf"

/* The error code returned if set_rlimits exits abnormally */
#define ERROR_RETURN_CODE   255

/* ======================================================================== */
/* Entity to hold rlimit values, associated defines and support structures */

/* Internal identifiers used for the resource limits.  These are also used
 * to index into the Rlimits type.  If this enum is changed note that
 * conf_file_keywords, rlimits_max, rlimits_cl_opt and rlimit_resource must
 * all be kept in sync.
 */
enum {
  RLIMITS_NICE = 0,
  RLIMITS_RTPRIO,
  RLIMITS_MEMLOCK,
  RLIMITS_NUM,
};
typedef signed int Rlimits[RLIMITS_NUM];

/* Keywords used to identify the resource limits in the config file */
const char *conf_file_keywords[] = {
  "nice", "rtprio", "memlock",
};
/* Define the maximum possible/supported values for each resource limit */
const signed int rlimits_max[] = {
  39, 100, 1048576,
};
/* Command line options used to request a specific resource limit */
const char *rlimits_cl_opt[] = {
  "n", "r", "l",
};
/* Mapping from limits defined by the internal RLIMITS_* identifier and the
 * kernel's RLIMIT_* flags.  Again this needs to be kept in sync with the
 * RLIMITS_* enum.
 */
const signed int rlimit_resource[] = {
  RLIMIT_NICE, RLIMIT_RTPRIO, RLIMIT_MEMLOCK,
};

#define RLIMITS_USE_DEFAULT -1
#define RLIMITS_NO_DEFAULT  -2

/* ======================================================================== */

signed int debug = 0;
signed int verbose = 0;

char *ldlibpath = NULL;

/* rlimits for process */
Rlimits rlimits = {
  RLIMITS_USE_DEFAULT, RLIMITS_USE_DEFAULT, RLIMITS_USE_DEFAULT,
};

/* ======================================================================== */

void show_usage() {

  fprintf(stderr,
    "set_rlimits " VERSION " - allow non-privileged access to elevated resource limits\n"
    "(c) 2005-2006 Jonathan Woithe.  Released under the terms of the GNU General\n"
    "Public License - no warranty is given or implied.  See source for full details.\n"
    "Usage: set_rlimits [options] <program> [<program options>]\n"
    "  Limits are controlled by the configuration file (" CONFIG_FILE ").\n"
    "  <program> is the full path to a program to execute after setting elevated\n"
    "  resource limits.  <program options> are options passed to <program>.\n"
    "  For convenience, <program> can be a simple path-less name, in which case the\n"
    "  full path is taken from the matching configuration file entry.\n"
    "  With no options, all resource limits with maximums defined for <program> in\n"
    "  the configuration file are set to those maximums.\n"
    "Available options:\n"
    " -d               Turn on additional debugging output.\n"
    " -h               Display this help message.\n"
    " -n[=<nlimit>]    Set nice limit to <nlimit>.  Values 0 to 39 equate to nice\n"
    "                  levels 19 to -20 respectively.\n"
    " -r[=<priority>]  Set realtime priority limit to <priority>.  Max available is\n"
    "                  %d.\n"
    " -l[=<mlock>]     Set max locked memory to <mlock> kB (max is %d kB).\n"
    " -L=<path>        Set LD_LIBRARY_PATH to <path> when running <program>.\n"
    " -v               Print version information\n"
    "\n"
    "rlimits activated on the command line default to the maximum set in the\n"
    "configuration file for the given program.  If an rlimit of -1 is requested,\n"
    "that rlimit will not be changed (ie: the default will not be used).\n"
    "\n" 
    "Exit code is %d if set_rlimits encountered an error, or the exit code of\n"
    "the executed program.\n",
    rlimits_max[RLIMITS_RTPRIO],rlimits_max[RLIMITS_MEMLOCK],
    ERROR_RETURN_CODE); 
}
/* ======================================================================== */

signed int parse_priority(signed int arg_index, int argc, char *argv[], signed int max_priority) {
/* 
 * Parses a priority number from the argument list.  The argument at
 * argv[arg_index] is assumed to be the option which triggers the search for
 * a priority.  Return value is the priority level specified,
 * RLIMITS_USE_DEFAULT (-1) to indicate that a default value should be used,
 * RLIMITS_NO_DEFAULT (-2) if no resource limit adjustment should be made,
 * or -3 to indicate an error.  max_priority allows this function to impose
 * an upper limit on the priority specified (the lower bound is always 0).
 *
 * A priority is specified after an = sign following the option letter.
 */
signed int res;
  /* No priority given, so use default value */
  if (argv[arg_index][2] == 0)
    return RLIMITS_USE_DEFAULT;

  /* Malformed option: flag an error */
  if (argv[arg_index][2] != '=')
    return -3;

  /* Scan the argument provided and sanity check */
  if (sscanf(argv[arg_index]+3,"%d",&res) != 1) 
    return -3;
  if (res <= -1)
    return RLIMITS_NO_DEFAULT;
  if (res<0 || res>max_priority) {
    fprintf(stderr,"set_rlimits: priority in `%s' option out of range (min = 0, max = %d)\n",
      argv[arg_index], max_priority);
    return -3;
  }
  return res;
}
/* ======================================================================== */

signed int parse_args(int argc, char *argv[], Rlimits rlimits) {
/*
 * Simplistically parses the command line arguments given.  Returns -1 on
 * error requiring usage information be displayed, -2 for an exit without
 * usage information or the argv element at which the options end (and the
 * program to execvp() starts).
 *
 * The results of any requested resource limits are returned in the
 * rlimits paramter.
 */
signed int id, i;
signed int err = 0;

  for (i=0;i<RLIMITS_NUM;i++)
    rlimits[i] = RLIMITS_USE_DEFAULT;

  i = 1;
  while (!err && i<argc && argv[i][0]=='-') {
    id = 0;
    while (id<RLIMITS_NUM && 
           (rlimits_cl_opt[id]==NULL || 
            rlimits_cl_opt[id][0]!=argv[i][1] ||
            (argv[i][2]!=0 && argv[i][2]!='=')))
      id++;
    /* Handle a request for a specific resource limit setting */
    if (id<RLIMITS_NUM) {
      rlimits[id] = parse_priority(i,argc,argv,rlimits_max[id]);
      if (rlimits[id] < 0 && rlimits[id]!=RLIMITS_NO_DEFAULT &&
          rlimits[id]!=RLIMITS_USE_DEFAULT)
        err = -2;
    } else {
      /* All other parameters handled here */
      switch (argv[i][1]) {
        case 'd': 
          if (argv[i][2]!=0)
            err = -1;
          else 
            debug = 1;
          break;
        case 'h': err = -1; break;
        case 'L':
          if (argv[i][2]!='=')
            err = -1;
          else
            ldlibpath = argv[i]+3;
          break;
        case 'v':
          if (argv[i][2]!=0)
            err = -1;
          else {
            fprintf(stderr,
              "set_rlimits " VERSION "\n"
            );
            err = -2;
          }
          break;
        default:
          err = -1;
      }
    }
    if (!err) 
      i++;
  }
  if (err && err!=-2 && argv[i][1]!='h') {
    fprintf(stderr,"set_rtlimit: unknown/erroneous option `%s'\n",argv[i]);
  }
  return err?err:i;
}
/* ======================================================================== */

char *skip_whitespace(char *c) {
/*
 * Skips leading whitespace in c and returns a pointer to the first character
 * after the whitespace.  If there are no non-whitespace characters, NULL
 * is returned.
 */
char *p = c;
  if (c==NULL)
    return NULL;
  while (*p!=0 && (*p==' ' || *p=='\t'))
    p++;
  if (*p==0)
    return NULL;
  return p;
}
/* ======================================================================== */

char *skip_to_whitespace(char *c) {
/*
 * Skips to the end of a non-whitespace sequence and returns a pointer to
 * the first whitespace character.  If there is no whitespace to be found,
 * NULL is returned.
 */
char *p = c;
  if (c==NULL)
    return NULL;
  while (*p!=0 && *p!=' ' && *p!='\t')
    p++;
  if (*p==0)
    return NULL;
  return p;
}
/* ======================================================================== */

static signed int read_priority(char *token, signed int *priority) {
/*
 * Reads a priority number from the given string.  It is assumed all leading
 * and trailing whitespace has been trimmed from token before this function
 * is called.  The priority is assumed to be a signed integer.
 *
 * Return value is 0 on success and -1 if there is an error in the string.
 */
signed int nc;

  if (sscanf(token,"%d%n",priority,&nc)<1 || *(token+nc)!=0) { 
    return -1;
  }
  return 0;
}
/* ======================================================================== */

signed int scan_conf_file_rlimits(char *line, signed int linenum, Rlimits rlimits) {
/*
 * Scans rlimits from a given line of the configuration file into the
 * supplied Rlimits structure.  rlimits is assumed to point to an
 * already-allocated Rlimits structure.  line is assumed to start with
 * rlimit values.
 *
 * Two formats are supported for "line": the old-style format used in
 * set_rtlimits <= v1.1.0, and the new-style format used in set_rlimits
 * 1.2.0 and later.
 *
 * The old-style entry consists of 2 signed integers separated by whitespace,
 * representing a maximum nice and realtime priority level respectively.
 *
 * The new format utilises whitespace separated keyword/value pairs of
 * the form
 *   <keyword>=<value>
 * No whitespace is permitted between the <keyword>, `=' sign or <value>.
 *
 * Returns 0 on success or -1 on error.  Error messages are output by this
 * function so there's no real need to return more detailed error codes.
 */
char *token, *c = skip_whitespace(line);
signed int keyword_id, n_values=0, err=0;
signed int old_format, priority;

  /* Initialise the rlimits return value */
  for (keyword_id=0;keyword_id<RLIMITS_NUM;keyword_id++)
    rlimits[keyword_id] = -1;
  /* Immediately return to caller if there's nothing to do */
  if (*line==0)
    return 0;
  /* Crudely detect the format of the line: if there's no equal signs
   * assume the old format. */
  old_format = (strchr(c,'=')==NULL);

  while (c!=NULL && *c!=0 && !err) {
    c = skip_whitespace(c);
    n_values++;
    /* The old format can have at most two values specified: nice and
     * realtime priority maximums.
     */
    if (old_format && n_values>2) {
      fprintf(stderr,"set_rlimits: error: line %d has too many priorities.  Aborting.\n", linenum);
      err=1;
      continue;
    }
    if (old_format) {
      token = skip_whitespace(c);
      if ((c=skip_to_whitespace(token))!=NULL)
        *(c++) = 0;
      if (read_priority(token,&priority) == -1) {
        fprintf(stderr,"set_rlimits: error: priority on line %d in %s corrupt\n",
          linenum, CONFIG_FILE);
        err = 1;
        continue;
      }
      /* n_values will be either 1 or 2 at this point, indicating that the
       * current priority value is (respectively) a nice or rt value.
       */
      keyword_id = (n_values==1)?RLIMITS_NICE:RLIMITS_RTPRIO;
    } else {
      /* Parse a new-format rlimit specification.  Ensure there is an `='
       * sign present. Currently this is assured due to the format
       * determination being based on the presence of such a character. 
       * However, include the test here as well in case this subtly is
       * overlooked if support for the old format is dropped some time in
       * the future.
       */
      token = strchr(c,'=');
      if (!token) {
        fprintf(stderr,"set_rlimits: error: missing `=' on line %d in %s\n",
          linenum, CONFIG_FILE);
        err = 1;
        continue; 
      }
      *token = 0;
      token++;  
      /* Identify the keyword */
      keyword_id = 0;
      while (keyword_id<RLIMITS_NUM && strcmp(c,conf_file_keywords[keyword_id]))
        keyword_id++;
      if (keyword_id == RLIMITS_NUM) {
        fprintf(stderr,"set_rlimits: error: unknown keyword `%s' on line %d of %s\n",
          c,linenum, CONFIG_FILE);
        err = 1;
        /* Since the while loop will exit due to error there's no need for
         * c to be updated prior to continuing.
         */
        continue;
      }
      /* Point c past the token to the start of the next keyword/value pair
       * (if it exists).  If there is something else after the current set,
       * insert a NULL character just after the current token so
       * read_priority() will work correctly.
       */
      c = skip_to_whitespace(token);
      if (c!=NULL)
        *(c++) = 0;
      /* Extract the (maximum) priority */
      if (read_priority(token,&priority) == -1) {
        fprintf(stderr,"set_rlimits: error: limit on line %d in %s corrupt\n",
          linenum, CONFIG_FILE);
        err = 1;
        continue;
      }
    }
 
    /* Everything checks out so assign the current priority value into the
     * rlimit parameter.  Map all negative values to RLIMITS_USE_DEFAULT  for
     * convenience and clarity later on.
     */
    if (priority < 0)
      priority = RLIMITS_USE_DEFAULT;
    rlimits[keyword_id] = priority;
  }

  return err?-1:0; 
}
/* ======================================================================== */

signed int check_user_match(char *user_str, uid_t uid, gid_t gid,
  signed int n_sgid, gid_t *sgid_list) {
/*
 * Checks to see if the user string (user_str) matches the user described by
 * the supplied uid, gid and supplemental groups.  user_str can be either a
 * username or (if preceded by a `@') a groupname.
 *
 * Returns 0 if there is a match, 1 if not, and -1 if the given user string
 * did not resolve to a known user/group name.
 */
struct passwd *pwbuf;
struct group *gbuf;
uid_t cf_uid = -1;
gid_t cf_gid = -1;
signed int i, match;

  /* Deal with the trivial "ALL" case first */
  if (!strcmp(user_str,"ALL")) {
    if (debug) {
      fprintf(stderr,"ALL user/group match detected\n");
    }
    return 0;
  }

  /* Now deal with a groupname if specified */
  if (user_str[0]=='@') {
    gbuf = getgrnam(user_str+1);
    if (gbuf == NULL) {
      return -1;
    }
    cf_gid = gbuf->gr_gid;
  } else {
    if ((pwbuf=getpwnam(user_str)) == NULL) {
      return -1;
    }
    cf_uid = pwbuf->pw_uid;
  }
  match = (cf_uid!=-1 && cf_uid==uid) || (cf_gid!=-1 && cf_gid==gid);

  /* If no direct match, check the gid from the configuration file (as
   * extracted from user_str) against the user's supplementary groups (if
   * available)
   */
  if (cf_gid!=-1 && n_sgid>0) {
    i=0;
    while (!match && i<n_sgid) {
      match = sgid_list[i]==cf_gid;
      i++;
    }
  }
  return match?0:1;
}
/* ======================================================================== */

signed int check_config_file(uid_t uid, gid_t gid, char **exec_path,
  Rlimits rlimits) {
/*
 * Checks to see if the current user has permission to execute the requested
 * program with the specified resource limits.  This information is
 * obtained from a fixed configuration file CONFIG_FILE.
 *
 * The configuration file is a simple textfile with lines of the form
 *   name  program  max_nice_priority  max_rt_priority
 * or
 *   name  program  rlimitname=value [rlimitname=value]...
 * name can be ALL which matches all users.  If name starts with a @ sign it
 * is taken as a group name; otherwise it is assumed to be a username. 
 * Where a given user could match multiple entries, the first matching entry
 * encountered is acted upon.  If a priority/value is <0 it effectively
 * disallows any setting of that resource limit when the entry is matched.
 *
 * On entry, *exec_path (which is the program the user wishes to run with
 * elevated resource limits) can be either include an absolute path to a
 * binary or be the name of a binary without any path information at all. 
 * If a matching entry is found by this function in the latter case,
 * exec_path will be replaced on exit with the full path to the binary as
 * specified in the matching configuration file entry.  To do this it is
 * assumed that *exec_path has been allocated using malloc().
 *
 * If a matching configuration file entry is found, the limits in the
 * supplied rlimits parameter are checked against the maximum/default
 * entries given in the configuration file.  If any out-of-bound values are
 * found, 0 will be returned.  Any limits specified as -1 (ie: use default
 * value) will be filled in with the value from the configuration file if it
 * is given.  Limits equal to -2 on entry (ie: do not set this resource)
 * will be set to -1 even if a value is given in the configuration file.  A
 * value of -1 on return indicates no elevated setting should be used for
 * the respective limit.
 *
 * Return value is 1 if everything is OK, 0 if the configuration file denies
 * access (by virtue of there being no matching entires or requested limits
 * being out of bounds), or -1 in case of error.  Error messages are generated
 * within this function, so there's no real need for the caller to know
 * the true cause of the error.
 */

FILE *cf_file;
signed int match=0, err=0, line=0;
signed int n_sgid = 0;
gid_t *sgid_list = NULL;
Rlimits cf_rlimits;

char buf[1024], *c;
char *user, *prog;
signed int i;

  /* Open configuration file to see if there's a matching entry */
  cf_file = fopen(CONFIG_FILE,"r");
  if (!cf_file) {
    fprintf(stderr,"set_rlimits: error: could not access configuration file %s to verify permissions\n",
      CONFIG_FILE);
    return -1;
  }

  /* Construct a list of the current user's supplementary groups.  Under 
   * Linux this appears to work as expected for setuid binaries being
   * run by a non-root user. 
   */
  n_sgid = getgroups(0,NULL);
  if (n_sgid != 0) {
    sgid_list = malloc(sizeof(gid_t)*n_sgid);
    if (sgid_list == NULL) {
      fprintf(stderr,"set_rlimits: error: out of memory.  Aborting.\n");
      return -1;
    }
    getgroups(n_sgid,sgid_list);
  }

  /* Scan configuration file for matching entry */
  do {
    line++;
    buf[0] = 0;
    fgets(buf,1024,cf_file);
    if (!strchr(buf,'\n') && !feof(cf_file)) {
      fprintf(stderr,"set_rlimits: error: line %d in %s is too long (maximum length is 1024 characters)\n",
        line, CONFIG_FILE);
      err = 1;
      continue;
    }
    /* Deal with newlines, comment characters, and then split the line into
     * its components
     */
    if ((c=strchr(buf,'\n')) != NULL)
      *c = 0;
    if ((c=strchr(buf,'#')) != NULL)
      *c = 0;
    if (buf[0] == 0)
      continue;

    /* Extract the user/group and program from the line followed by rlimit
     * details.
     */
    user = skip_whitespace(buf);
    if ((c=skip_to_whitespace(user))!=NULL)
      *(c++) = 0;
    prog = skip_whitespace(c);
    if ((c=skip_to_whitespace(prog))!=NULL)
      *(c++) = 0;

    /* If the line is in error, flag the error and abort */
    if (!user || !prog || !c) {
      fprintf(stderr,"set_rlimits: error: line %d in %s is not complete\n",
        line, CONFIG_FILE);
      err = 1;
      continue;
    }
    /* The program specified in the configuration program must include an
     * absolute path for security reasons.
     */
    if (prog[0] != '/') {
      fprintf(stderr,"set_rlimits: warning: program `%s' on line %d in %s does not include an absolute path.  Entry ignored.\n",
        prog, line, CONFIG_FILE);
      continue;
    }

    /* Parse the priorities given in the current configuration file line
     * and abort if an error was encountered.  This is done at this point
     * so any errors in the configuration file are picked up and not masked
     * just because the user specification doesn't match the user running
     * the program.
     */
    if (scan_conf_file_rlimits(c,line,cf_rlimits) != 0) {
      err=1;
      continue;
    }

    /* Deal with the program name.  *exec_path can be either an absolute
     * pathname to a binary OR a binary name without *any* path information. 
     * The latter case is for user convenience; if a match is found this way
     * the path to the binary ultimately run will come from the set_rlimits
     * configuration file.  Continue to next configuration file entry if
     * there's no match either way.
     */
    if ((strchr(*exec_path,'/')!=NULL && strcmp(prog,*exec_path)) ||
        (strchr(*exec_path,'/')==NULL && strcmp(strrchr(prog,'/')+1,*exec_path)))
      continue;

    /* At this point see if the user specified on the current configuration 
     * file line matches the user running this program.
     */
    i = check_user_match(user,uid,gid,n_sgid,sgid_list);
    if (i==-1) {
      fprintf(stderr,"set_rlimits: warning: %sname `%s' on line %d of %s is not a valid %s.  Entry ignored.\n",
        user[0]=='@'?"group":"user",user[0]=='@'?user+1:user,line,
        CONFIG_FILE,user[0]=='@'?"group":"user");
    } else
      match = i==0;

  } while (!err && !match && !feof(cf_file));
  fclose(cf_file);
  
  /* The list of supplementary groups is no longer needed */
  if (sgid_list != NULL)
    free(sgid_list);

  /* Flag error in configuration file */
  if (err) {
    return -1;
  }
  /* No matching entry - permission denied */
  if (!match) {
    fprintf(stderr,"set_rlimits: no permission to run %s with elevated resource limits\n",
      *exec_path);
    return 0;
  }

  if (debug) {
    fprintf(stderr,"Matching config file entry: user/group=%s, prog=%s, max_nice=%d, max_rt=%d, max_mlock=%d\n",
      user,prog, cf_rlimits[RLIMITS_NICE], cf_rlimits[RLIMITS_RTPRIO],
      cf_rlimits[RLIMITS_MEMLOCK]);
  }

  /* Check maximum allowed priorities given in configuration file entry and
   * fill in defaults if requested.
   */
  for (i=0;i<RLIMITS_NUM;i++) {
    if (rlimits[i]==RLIMITS_NO_DEFAULT)
      continue;
    if (rlimits[i]==RLIMITS_USE_DEFAULT)
      rlimits[i] = cf_rlimits[i];
    else
    if (cf_rlimits[i] == -1) {
      fprintf(stderr,"set_rlimits: Setting of %s limit for %s is not permitted\n",
        conf_file_keywords[i], prog);
      return 0;
    } else
    if (rlimits[i]>cf_rlimits[i]) {
      fprintf(stderr,"set_rlimits: Requested %s limit (%d) not permitted for %s (maximum allowed is %d)\n",
        conf_file_keywords[i],rlimits[i], prog, cf_rlimits[i]);
      return 0;
    }
  }

  /* Everything appears in order, so give the OK.  Before exitting ensure
   * that *exec_path contains a full pathname as specified in the matching
   * configuration file entry. 
   */
  if (*exec_path[0] != '/') {
    free(*exec_path);
    *exec_path = strdup(prog);
    /* Abort if there was an error allocating memory */
    if (*exec_path == NULL) {
      fprintf(stderr,"set_limits: error: out of memory.  Aborting.\n");
      return 0;
    }
  }

  return 1;
}
/* ======================================================================== */

signed int check_ld_path(const char *path) {
/*
 * Checks the ownership of all directories in the given path.  The directory
 * separator is ":".  To be acceptable, root (or more accurately, uid 0)
 * must own the directory and there must be no group/other write permission. 
 * This prevents a user replacing a standard library with their own in order
 * to subvert a process started with set_rlimits.
 *
 * Return value is 1 if everything is OK, or 0 if not.
 */
char *path_copy, *p, *sep;
struct stat info;
signed int ok = 1;
  if (path==NULL || *path==0)
    return 1;

  p = path_copy = strdup(path);
  while (p!=NULL && *p!=0 && ok) {
    sep = strchr(p,':');
    if (sep != NULL)
      *sep = 0;
    if (stat(p, &info) != 0)
      ok = 0;
    else {
      ok = info.st_uid==0 && ((info.st_mode & (S_IWGRP|S_IWOTH))==0);
    }
    if (sep != NULL)
      p = sep+1;
    else
      p = NULL;
  }
  free(path_copy);

  return ok;
}
/* ======================================================================== */

int main(int argc, char *argv[]) {

struct rlimit rlim;
signed int result, prog_argv_index;
signed int setuid_root = 1;
signed int i, num_rlimits_set;
uid_t uid, euid;
gid_t gid, egid;
char *prog_to_exec = NULL;

  uid = getuid();
  gid = getgid();
  euid = geteuid();
  egid = getegid();

  if (uid!=0 && euid!=0) {
    fprintf(stderr,"set_rlimits: warning: not running setuid root, cannot set resource limits.\n"
      "  Currently running with uid=%d, euid=%d\n",uid,euid);
    setuid_root = 0;
  }

  /* Drop root privileges for the moment */
  setregid(egid, gid);
  setreuid(euid, uid);

  /* Work out what to do and handle usage errors */
  prog_argv_index = parse_args(argc, argv, rlimits);
  if (prog_argv_index < 0) {
    if (prog_argv_index == -1) 
      show_usage();
    return ERROR_RETURN_CODE;
  }
  if (prog_argv_index == argc) {
    fprintf(stderr,"set_rlimits: no specified program to execute - nothing to do\n");
    return ERROR_RETURN_CODE;
  }

  prog_to_exec = strdup(argv[prog_argv_index]);
  if (prog_to_exec == NULL) {
    fprintf(stderr,"set_limits: error: out of memory.  Aborting.\n");
    return ERROR_RETURN_CODE;
  }
  result = check_config_file(uid,gid,&prog_to_exec,rlimits);
  if (result != 1) {
    return ERROR_RETURN_CODE;
  }

  /* Debug: explore selected current resource limit settings */
  if (debug) {
    for (i=0; i<RLIMITS_NUM; i++) {
      result = getrlimit(rlimit_resource[i],&rlim);
      fprintf(stderr,"`%s' getrlimit returned %d;   ",conf_file_keywords[i],
        result);
      /* Almost everything - including this program - deals with the memlock
       * limit in terms of kB, so scale it if the limit being dealt with
       * is the memlock limit.
       */
      if (i == RLIMITS_MEMLOCK) {
        rlim.rlim_cur /= 1024;
        rlim.rlim_max /= 1024;
      }
      fprintf(stderr,"current `%s' rlimits: soft=%u, hard=%u\n",
        conf_file_keywords[i],
        (unsigned int)rlim.rlim_cur,(unsigned int)rlim.rlim_max);
    }
  }

  /* Regain root privileges to set the resource limits */
  setregid(gid,egid);
  setreuid(uid,euid);

  /* Now try to set each resource limit as requested */
  num_rlimits_set = 0;
  for (i=0;i<RLIMITS_NUM;i++) {
    if (rlimits[i] < 0)
      continue;
    rlim.rlim_max = rlim.rlim_cur = rlimits[i];
    /* For the memlock limit, setrlimit() expects the limit to be in bytes.
     * Therefore convert the limits from kB to bytes if we're dealing with
     * the memlock limit.
     */
    if (i == RLIMITS_MEMLOCK) {
      rlim.rlim_max *= 1024;
      rlim.rlim_cur *= 1024;
    }
    result = setrlimit(rlimit_resource[i],&rlim);
    if (result<0) {
      fprintf(stderr,"set_rlimits: error setting %s resource limit: %s\n",
        conf_file_keywords[i],strerror(errno));
      if (setuid_root && (i==RLIMITS_NICE || i==RLIMITS_RTPRIO)) {
        fprintf(stderr,"  Your kernel probably doesn't have %s support; try 2.6.12 or later\n",
          i==RLIMITS_NICE?"RLIMIT_NICE":"RLIMIT_RTPRIO");
      }
    } else
      num_rlimits_set++;
    if (debug) {
      fprintf(stderr,"`%s' rlimit requested: %d\n",conf_file_keywords[i],rlimits[i]);
      fprintf(stderr,"`%s' setrlimit returned %d",conf_file_keywords[i],result);
      if (result<0) {
        fprintf(stderr,": errno = %d",errno);
      }
      fprintf(stderr,"\n");
    }
  }

  /* Drop root privileges for good */
  setregid(gid, gid);
  setreuid(uid, uid);

  if (num_rlimits_set == 0) {
    fprintf(stderr,"set_rlimits: warning: no resource limit changes made; running set_rlimits\n"
                   "  will have no effect.\n");
  }

  /* Finally, report selected resource limits after set_rlimits has run */
  if (debug) {
    for (i=0; i<RLIMITS_NUM; i++) {
      result = getrlimit(rlimit_resource[i],&rlim);
      fprintf(stderr,"`%s' getrlimit returned %d; ",conf_file_keywords[i],
        result);
      /* Scale the memlock rlimits into kB for display */
      if (i == RLIMITS_MEMLOCK) {
        rlim.rlim_cur /= 1024;
        rlim.rlim_max /= 1024;
      }
      fprintf(stderr,"new `%s' rlimits: soft=%u, hard=%u\n",
        conf_file_keywords[i],
        (unsigned int)rlim.rlim_cur,(unsigned int)rlim.rlim_max);
    }
  }

  /* If requested on the command line, set up LD_LIBRARY_PATH for the executed
   * binary.
   */
  if (ldlibpath != NULL) {
    if (check_ld_path(ldlibpath)) {
      fprintf(stderr,"set_rlimits: warning: setting LD_LIBRARY_PATH to `%s' for %s\n",
        ldlibpath, prog_to_exec);
      setenv("LD_LIBRARY_PATH", ldlibpath, 0);
    } else {
      fprintf(stderr,"set_rlimits: warning: not setting LD_LIBRARY_PATH due to insecure permissions\n"
        "  of at least one component\n");
    }
  }

  /* Now execute the main target application with its parameters.
   * Fiddling argv like this to ensure the appropriate program path is
   * in argv[0] of the executed process is probably not the neatest 
   * solution to the problem but it appears to work. 
   */
  argv[prog_argv_index] = prog_to_exec;
  execvp(prog_to_exec, argv+prog_argv_index);

  fprintf(stderr,"set_rlimits: error: could not exec `%s': %s\n",
    prog_to_exec,strerror(errno));

  return ERROR_RETURN_CODE;
}
