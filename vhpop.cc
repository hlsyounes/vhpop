/*
 * Main program.
 *
 * Copyright (C) 2002 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: vhpop.cc,v 3.18 2002-06-26 23:34:25 lorens Exp $
 */
#include "plans.h"
#include "reasons.h"
#include "parameters.h"
#include "heuristics.h"
#include "domains.h"
#include "problems.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_GETOPT_LONG
#define _GNU_SOURCE
#include <getopt.h>
#else
#include "getopt.h"
#endif


/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;
/* Name of current file. */
extern string current_file;
/* Level of warnings. */
extern int warning_level;

/* Verbosity level. */
int verbosity;


/* Program options. */
static struct option long_options[] = {
  { "domain-constraints", optional_argument, NULL, 'd' },
  { "flaw-order", required_argument, NULL, 'f' },
  { "ground-actions", no_argument, NULL, 'g' },
  { "heuristic", required_argument, NULL, 'h' },
  { "limit", required_argument, NULL, 'l' },
  { "reverse-open-conditions", no_argument, NULL, 'r' },
  { "search-algorithm", required_argument, NULL, 's' },
#ifdef TRANSFORMATIONAL
  { "transformational", no_argument, NULL, 't' },
#endif
  { "time-limit", required_argument, NULL, 'T' },
  { "verbose", optional_argument, NULL, 'v' },
  { "version", no_argument, NULL, 'V' },
  { "weight", required_argument, NULL, 'w' },
  { "warnings", optional_argument, NULL, 'W' },
  { "help", no_argument, NULL, '?' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "d::f:gh:l:rs:tT:v::Vw:W::?";


/* Displays help. */
static void display_help() {
  cout << "usage: " << PACKAGE << " [options] [file ...]" << endl
       << "options:" << endl
       << "  -d[k], --domain-constraints=[k]" << endl
       << "\t\t\tuse parameter domain constraints;" << endl
       << "\t\t\t  if k is 0, static preconditions are pruned;" << endl
       << "\t\t\t  otherwise (default) static preconditions are kept" << endl
       << "  -f f,  --flaw-order=f\t"
       << "use flaw selection order f" << endl
       << "  -g,    --ground-actions" << endl
       << "\t\t\tuse ground actions" << endl
       << "  -h h,  --heuristic=h\t"
       << "use heuristic h to rank plans" << endl
       << "  -l l,  --limit=l\t"
       << "search no more than l plans" << endl
       << "  -r,    --reverse-open-conditions" << endl
       << "\t\t\treverse the order that open conditions are added" << endl
       << "  -s s,  --search-algorithm=s" << endl
       << "\t\t\tuse search algorithm s" << endl
#ifdef TRANSFORMATIONAL
       << "  -t,    --transformational" << endl
       << "\t\t\tuse transformational planner" << endl
#endif
       << "  -T t,  --time-limit=t\t"
       << "limit search to t minutes" << endl
       << "  -v[n], --verbose[=n]\t"
       << "use verbosity level n;" << endl
       << "\t\t\t  n is a number from 0 (verbose mode off) and up;" << endl
       << "\t\t\t  default level is 1 if optional argument is left out" << endl
       << "  -V,    --version\t"
       << "display version information and exit" << endl
       << "  -w,    --weight=w\t"
       << "weight to use with heuristic (default is 1)" << endl
       << "  -W[n], --warnings[=n]\t"
       << "determines how warnings are treated;" << endl
       << "\t\t\t  0 supresses warnings; 1 displays warnings;" << endl
       << "\t\t\t  2 treats warnings as errors" << endl
       << "  -?     --help\t\t"
       << "display this help and exit" << endl
       << "  file ...\t\t"
       << "files containing domain and problem descriptions;" << endl
       << "\t\t\t  if none, descriptions are read from standard input" << endl
       << endl
       << "Report bugs to " PACKAGE_BUGREPORT "." << endl;
}


/* Displays version information. */
static void display_version() {
  cout << PACKAGE " " VERSION << endl
       << "Copyright (C) 2002 Carnegie Mellon University" << endl
       << PACKAGE " comes with NO WARRANTY," << endl
       << "to the extent permitted by law." << endl
       << "Written by H\345kan L. S. Younes." << endl;
}


/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  yyin = fopen(name, "r");
  if (yyin == NULL) {
    cerr << PACKAGE << ':' << name << ": " << strerror(errno) << endl;
    return false;
  } else {
    current_file = name;
    bool success = (yyparse() == 0);
    fclose(yyin);
    return success;
  }
}


/* Cleanup function. */
static void cleanup() {
  Problem::clear();
  Domain::clear();

#ifdef DEBUG_MEMORY
  cerr << "Plans created: " << created_plans << endl;
  cerr << "Plans deleted: " << deleted_plans << endl;
  cerr << "Chains created: " << created_chains << endl;
  cerr << "Chains deleted: " << deleted_chains << endl;
  cerr << "Collectibles created: " << created_collectibles << endl;
  cerr << "Collectibles deleted: " << deleted_collectibles << endl;
#endif
}


#ifdef DEBUG_MEMORY
size_t created_plans = 0;
size_t deleted_plans = 0;
size_t created_chains = 0;
size_t deleted_chains = 0;
size_t created_collectibles = 0;
size_t deleted_collectibles = 0;
#endif


/* The main program. */
int main(int argc, char* argv[]) {
  atexit(cleanup);

  /* Default planning parameters. */
  Parameters params;
  /* Set default verbosity. */
  verbosity = 0;
  /* Set default warning level. */
  warning_level = 1;

  /*
   * Get command line options.
   */
  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, OPTION_STRING,
			long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'd':
      params.domain_constraints = true;
      params.keep_static_preconditions = (optarg == NULL || atoi(optarg) != 0);
      break;
    case 'f':
      try {
	params.flaw_order = optarg;
      } catch (const InvalidFlawSelectionOrder& e) {
	cerr << PACKAGE << ": " << e << endl
	     << "Try `" << PACKAGE << " --help' for more information."
	     << endl;
	return -1;
      }
      break;
    case 'g':
      params.ground_actions = true;
      break;
    case 'h':
      try {
	params.heuristic = optarg;
      } catch (const InvalidHeuristic& e) {
	cerr << PACKAGE << ": " << e << endl
	     << "Try `" << PACKAGE << " --help' for more information."
	     << endl;
	return -1;
      }
      break;
    case 'l':
      params.search_limit = atoi(optarg);
      break;
    case 'r':
      params.reverse_open_conditions = true;
      break;
    case 's':
      try {
	params.set_search_algorithm(optarg);
      } catch (const InvalidSearchAlgorithm& e) {
	cerr << PACKAGE << ": " << e << endl
	     << "Try `" << PACKAGE << " --help' for more information."
	     << endl;
	return -1;
      }
      break;
#ifdef TRANSFORMATIONAL
    case 't':
      params.transformational = true;
      break;
#endif
    case 'T':
      params.time_limit = atoi(optarg);
      break;
    case 'v':
      verbosity = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case 'V':
      display_version();
      return 0;
    case 'w':
      params.weight = atof(optarg);
      break;
    case 'W':
      warning_level = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case '?':
      if (optopt == '?') {
	display_help();
	return 0;
      }
    case ':':
    default:
      cerr << "Try `" << PACKAGE << " --help' for more information."
	   << endl;
      return -1;
    }
  }
  /* set the random seed. */
  srand(time(NULL));

  try {
    /*
     * Read pddl files.
     */
    if (optind < argc) {
      /*
       * Use remaining command line arguments as file names.
       */
      while (optind < argc) {
	if (!read_file(argv[optind++])) {
	  return -1;
	}
      }
    } else {
      /*
       * No remaining command line argument, so read from standard input.
       */
      yyin = stdin;
      if (yyparse() != 0) {
	return -1;
      }
    }

    if (verbosity > 1) {
      /*
       * Display domains and problems.
       */
      cerr << "----------------------------------------"<< endl
	   << "domains:" << endl;
      for (Domain::DomainMapIter di = Domain::begin();
	   di != Domain::end(); di++) {
	cerr << endl << *(*di).second << endl;
      }
      cerr << "----------------------------------------"<< endl
	   << "problems:" << endl;
      for (Problem::ProblemMapIter pi = Problem::begin();
	   pi != Problem::end(); pi++) {
	cerr << endl << *(*pi).second << endl;
      }
      cerr << "----------------------------------------"<< endl;
    }

    cerr.setf(ios::unitbuf);

    /*
     * Solve the problems.
     */
    for (Problem::ProblemMapIter pi = Problem::begin();
	 pi != Problem::end(); ) {
      const Problem& problem = *(*pi).second;
      pi++;
      cout << ';' << problem.name() << endl;
      struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
      setitimer(ITIMER_PROF, &timer, NULL);
#endif
#ifdef ALWAYS_DELETE_ALL
      const Plan* plan = Plan::plan(problem, params, false);
#else
      const Plan* plan = Plan::plan(problem, params, pi == Problem::end());
#endif
      if (plan != NULL) {
	if (plan->complete()) {
	  if (verbosity > 0) {
#ifdef DEBUG
	    cerr << "Depth of solution: " << plan->depth() << endl;
#endif
	  }
	  cout << *plan << endl;
	} else {
	  cout << "no plan" << endl;
	  cout << ";Search limit reached." << endl;
	}
	delete plan;
      } else {
	cout << "no plan" << endl;
	cout << ";Problem has no solution." << endl;
      }
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      /* Planning time. */
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      cout << "Time: " << max(0, int(1000.0*t + 0.5)) << endl;
    }
  } catch (const Exception& e) {
    cerr << PACKAGE << ": " << e << endl;
    return -1;
  } catch (...) {
    cerr << PACKAGE << ": fatal error" << endl;
    return -1;
  }

  return 0;
}
