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
 * $Id: vhpop.cc,v 3.5 2002-03-23 15:24:31 lorens Exp $
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/time.h>
#ifdef HAVE_GETOPT_LONG
#define _GNU_SOURCE
#include <getopt.h>
#else
#include <unistd.h>
#endif
#include "domains.h"
#include "problems.h"
#include "plans.h"
#include "orderings.h"
#include "heuristics.h"
#include "parameters.h"
#include "debug.h"


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


#ifdef HAVE_GETOPT_LONG
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
  { "no-gc", no_argument, NULL, 1 },
  { 0, 0, 0, 0 }
};
#endif
static const char OPTION_STRING[] = "d::f:gh:l:rs:tT:v::Vw:W::?";


/* Displays help. */
static void display_help() {
  cout << PROGRAM_NAME << endl
       << "usage: " << PROGRAM_NAME << " [options] [file ...]" << endl
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
       << "\t\t\t  if none, descriptions are read from standard input" << endl;
}


/* Displays version information. */
static void display_version() {
  cout << PROGRAM_NAME " " VERSION_NUMBER << " (" << __DATE__ << ")" << endl
       << "Written by H\345kan L. S. Younes (lorens@cs.cmu.edu)." << endl;
}


/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  yyin = fopen(name, "r");
  if (yyin == NULL) {
    cerr << PROGRAM_NAME << ':' << name << ": " << sys_errlist[errno] << endl;
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
}


#ifdef DEBUG
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
#ifdef HAVE_GETOPT_LONG
    int option_index = 0;
    int c = getopt_long(argc, argv, OPTION_STRING,
			long_options, &option_index);
#else
    int c = getopt(argc, argv, OPTION_STRING);
#endif
    if (c == -1) {
      break;
    }
    switch (c) {
    case 1:
      GC_dont_gc = 1;
      break;
    case 'd':
      params.domain_constraints = true;
      params.keep_static_preconditions = (optarg == NULL || atoi(optarg) != 0);
      break;
    case 'f':
      try {
	params.flaw_order = optarg;
      } catch (const InvalidFlawSelectionOrder& e) {
	cerr << PROGRAM_NAME << ": " << e << endl
	     << "Try `" << PROGRAM_NAME << " --help' for more information."
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
	cerr << PROGRAM_NAME << ": " << e << endl
	     << "Try `" << PROGRAM_NAME << " --help' for more information."
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
	cerr << PROGRAM_NAME << ": " << e << endl
	     << "Try `" << PROGRAM_NAME << " --help' for more information."
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
      cerr << "Try `" << PROGRAM_NAME << " --help' for more information."
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
      cout << ";----------------------------------------"<< endl
	   << ";domains:" << endl;
      for (Domain::DomainMapIter di = Domain::begin();
	   di != Domain::end(); di++) {
	cout << ';' << endl << ';' << *(*di).second << endl;
      }
      cout << ";----------------------------------------"<< endl
	   << ";problems:" << endl;
      for (Problem::ProblemMapIter pi = Problem::begin();
	   pi != Problem::end(); pi++) {
	cout << ';' << endl << ';' << *(*pi).second << endl;
      }
      cout << ";----------------------------------------"<< endl;
    }

    cout.setf(ios::unitbuf);

    /*
     * Solve the problems.
     */
    for (Problem::ProblemMapIter pi = Problem::begin();
	 pi != Problem::end(); pi++) {
      const Problem& problem = *(*pi).second;
      cout << ';' << problem.name << endl;
      struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
      setitimer(ITIMER_PROF, &timer, NULL);
      const Plan* plan = Plan::plan(problem, params);
      getitimer(ITIMER_PROF, &timer);
      /* Planning time. */
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      cout << "Time: " << max(0, int(1000.0*t + 0.5)) << endl;
      if (plan != NULL) {
	if (plan->complete()) {
	  if (verbosity > 0) {
#ifdef DEBUG
	    cout << ";Depth of solution: " << plan->depth() << endl;
#endif
	    const BinaryOrderings* binary_ords =
	      dynamic_cast<const BinaryOrderings*>(&plan->orderings());
	    if (binary_ords != NULL) {
	      cout << ";Flexibility: " << binary_ords->flexibility() << endl;
	    }
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
    }
  } catch (const Exception& e) {
    cerr << PROGRAM_NAME << ": " << e << endl;
    return -1;
  } catch (...) {
    cerr << PROGRAM_NAME << ": fatal error" << endl;
    return -1;
  }

#ifdef DEBUG
  cout << ";Chains created: " << created_chains << endl;
  cout << ";Chains deleted: " << deleted_chains << endl;
  cout << ";Collectibles created: " << created_collectibles << endl;
  cout << ";Collectibles deleted: " << deleted_collectibles << endl;
#endif

  return 0;
}
