/*
 * Main program.
 *
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: vhpop.cc,v 3.30 2003-03-01 23:45:53 lorens Exp $
 */
#include "plans.h"
#include "reasons.h"
#include "parameters.h"
#include "heuristics.h"
#include "domains.h"
#include "problems.h"
#include "debug.h"
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/time.h>
#if HAVE_GETOPT_LONG
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#else
#include "getopt.h"
#endif

#ifdef ALWAYS_DELETE_ALL
#define ALWAY_DELETE_ALL 1
#else
#define ALWAYS_DELETE_ALL 0
#endif


/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;
/* Name of current file. */
extern std::string current_file;
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
  { "tolerance", required_argument, NULL, 't' },
#ifdef TRANSFORMATIONAL
  { "transformational", no_argument, NULL, 1 },
#endif
  { "time-limit", required_argument, NULL, 'T' },
  { "verbose", optional_argument, NULL, 'v' },
  { "version", no_argument, NULL, 'V' },
  { "weight", required_argument, NULL, 'w' },
  { "warnings", optional_argument, NULL, 'W' },
  { "help", no_argument, NULL, '?' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "d::f:gh:l:rs:S:t:T:v::Vw:W::?";


/* Displays help. */
static void display_help() {
  std::cout << "usage: " << PACKAGE << " [options] [file ...]" << std::endl
	    << "options:" << std::endl
	    << "  -d[k], --domain-constraints=[k]" << std::endl
	    << "\t\t\tuse parameter domain constraints;" << std::endl
	    << "\t\t\t  if k is 0, static preconditions are pruned;"
	    << std::endl
	    << "\t\t\t  otherwise (default) static preconditions are kept"
	    << std::endl
	    << "  -f f,  --flaw-order=f\t"
	    << "use flaw selection order f" << std::endl
	    << "  -g,    --ground-actions" << std::endl
	    << "\t\t\tuse ground actions" << std::endl
	    << "  -h h,  --heuristic=h\t"
	    << "use heuristic h to rank plans" << std::endl
	    << "  -l l,  --limit=l\t"
	    << "search no more than l plans" << std::endl
	    << "  -r,    --random-open-conditions" << std::endl
	    << "\t\t\tadd open conditions in random order"
	    << std::endl
	    << "  -s s,  --search-algorithm=s" << std::endl
	    << "\t\t\tuse search algorithm s" << std::endl
	    << "  -S s,  --seed=s\t"
	    << "uses s as seed for random number generator" << std::endl
	    << "  -t t,  --tolerance=t\t"
	    << "use tolerance t with durative actions;" << std::endl
	    << "\t\t\t  time stamps less than t appart are considered"
	    << std::endl
	    << "\t\t\t  indistinguishable (default is 0.01)" << std::endl
#ifdef TRANSFORMATIONAL
	    << "         --transformational" << std::endl
	    << "\t\t\tuse transformational planner" << std::endl
#endif
	    << "  -T t,  --time-limit=t\t"
	    << "limit search to t minutes" << std::endl
	    << "  -v[n], --verbose[=n]\t"
	    << "use verbosity level n;" << std::endl
	    << "\t\t\t  n is a number from 0 (verbose mode off) and up;"
	    << std::endl
	    << "\t\t\t  default level is 1 if optional argument is left out"
	    << std::endl
	    << "  -V,    --version\t"
	    << "display version information and exit" << std::endl
	    << "  -w,    --weight=w\t"
	    << "weight to use with heuristic (default is 1)" << std::endl
	    << "  -W[n], --warnings[=n]\t"
	    << "determines how warnings are treated;" << std::endl
	    << "\t\t\t  0 supresses warnings; 1 displays warnings;"
	    << std::endl
	    << "\t\t\t  2 treats warnings as errors" << std::endl
	    << "  -?     --help\t\t"
	    << "display this help and exit" << std::endl
	    << "  file ...\t\t"
	    << "files containing domain and problem descriptions;" << std::endl
	    << "\t\t\t  if none, descriptions are read from standard input"
	    << std::endl
	    << std::endl
	    << "Report bugs to <" PACKAGE_BUGREPORT ">." << std::endl;
}


/* Displays version information. */
static void display_version() {
  std::cout << PACKAGE_STRING << std::endl
	    << "Copyright (C) 2003 Carnegie Mellon University" << std::endl
	    << PACKAGE_NAME
	    << " comes with NO WARRANTY, to the extent permitted by law."
	    << std::endl
	    << "For information about the terms of redistribution,"
	    << std::endl
	    << "see the file named COPYING in the " PACKAGE_NAME
	    << " distribution." << std::endl
	    << std::endl
	    << "Written by H\345kan L. S. Younes." << std::endl;
}


/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  yyin = fopen(name, "r");
  if (yyin == NULL) {
    std::cerr << PACKAGE << ':' << name << ": " << strerror(errno)
	      << std::endl;
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
  std::cerr << "Variables created: " << created_variables << std::endl
	    << "Variables deleted: " << deleted_variables << std::endl
	    << "Formulas created: " << created_formulas << std::endl
	    << "Formulas deleted: " << deleted_formulas << std::endl
	    << "Name sets created: " << created_name_sets << std::endl
	    << "Name sets deleted: " << deleted_name_sets << std::endl
	    << "Action domains created: " << created_action_domains
	    << std::endl
	    << "Action domains deleted: " << deleted_action_domains
	    << std::endl
	    << "Bindings created: " << created_bindings << std::endl
	    << "Bindings deleted: " << deleted_bindings << std::endl
	    << "Bool vectors created: " << created_bool_vectors << std::endl
	    << "Bool vectors deleted: " << deleted_bool_vectors << std::endl
	    << "Float vectors created: " << created_float_vectors << std::endl
	    << "Float vectors deleted: " << deleted_float_vectors << std::endl
	    << "Orderings created: " << created_orderings << std::endl
	    << "Orderings deleted: " << deleted_orderings << std::endl
	    << "Plans created: " << created_plans << std::endl
	    << "Plans deleted: " << deleted_plans << std::endl
	    << "Chains created: " << created_chains << std::endl
	    << "Chains deleted: " << deleted_chains << std::endl
	    << "Reasons created: " << created_reasons << std::endl
	    << "Reasons deleted: " << deleted_reasons << std::endl;
#endif
}


#ifdef DEBUG_MEMORY
size_t created_variables = 0;
size_t deleted_variables = 0;
size_t created_formulas = 0;
size_t deleted_formulas = 0;
size_t created_name_sets = 0;
size_t deleted_name_sets = 0;
size_t created_action_domains = 0;
size_t deleted_action_domains = 0;
size_t created_bindings = 0;
size_t deleted_bindings = 0;
size_t created_bool_vectors = 0;
size_t deleted_bool_vectors = 0;
size_t created_float_vectors = 0;
size_t deleted_float_vectors = 0;
size_t created_orderings = 0;
size_t deleted_orderings = 0;
size_t created_plans = 0;
size_t deleted_plans = 0;
size_t created_chains = 0;
size_t deleted_chains = 0;
size_t created_reasons = 0;
size_t deleted_reasons = 0;
#endif


/* The main program. */
int main(int argc, char* argv[]) {
  atexit(cleanup);

  /* Default planning parameters. */
  Parameters params;
  bool no_flaw_order = true;
  bool no_search_limit = true;
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
	if (no_flaw_order) {
	  params.flaw_orders.clear();
	  no_flaw_order = false;
	}
	params.flaw_orders.push_back(FlawSelectionOrder(optarg));
      } catch (const InvalidFlawSelectionOrder& e) {
	std::cerr << PACKAGE << ": " << e << std::endl
	     << "Try `" << PACKAGE << " --help' for more information."
	     << std::endl;
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
	std::cerr << PACKAGE ": " << e << std::endl
		  << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return -1;
      }
      break;
    case 'l':
      if (no_search_limit) {
	params.search_limits.clear();
	no_search_limit = false;
      }
      if (optarg == std::string("unlimited")) {
	params.search_limits.push_back(UINT_MAX);
      } else {
	params.search_limits.push_back(atoi(optarg));
      }
      break;
    case 'r':
      params.random_open_conditions = true;
      break;
    case 's':
      try {
	params.set_search_algorithm(optarg);
      } catch (const InvalidSearchAlgorithm& e) {
	std::cerr << PACKAGE ": " << e << std::endl
		  << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return -1;
      }
      break;
    case 'S':
      srand(atoi(optarg));
      break;
    case 't':
      if (optarg == std::string("unlimited")) {
	TemporalOrderings::threshold = UINT_MAX;
      } else {
	TemporalOrderings::threshold = atof(optarg);
      }
      break;
#ifdef TRANSFORMATIONAL
    case 1:
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
      std::cerr << "Try `" PACKAGE " --help' for more information."
		<< std::endl;
      return -1;
    }
  }
  for (size_t i = 0;
       i < params.flaw_orders.size() - params.search_limits.size(); i++) {
    params.search_limits.push_back(params.search_limits.back());
  }

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
      std::cerr << "----------------------------------------"<< std::endl
		<< "domains:" << std::endl;
      for (Domain::DomainMapIter di = Domain::begin();
	   di != Domain::end(); di++) {
	std::cerr << std::endl << *(*di).second << std::endl;
      }
      std::cerr << "----------------------------------------"<< std::endl
		<< "problems:" << std::endl;
      for (Problem::ProblemMapIter pi = Problem::begin();
	   pi != Problem::end(); pi++) {
	std::cerr << std::endl << *(*pi).second << std::endl;
      }
      std::cerr << "----------------------------------------"<< std::endl;
    }

    std::cerr.setf(std::ios::unitbuf);

    /*
     * Solve the problems.
     */
    for (Problem::ProblemMapIter pi = Problem::begin();
	 pi != Problem::end(); ) {
      const Problem& problem = *(*pi).second;
      pi++;
      std::cout << ';' << problem.name() << std::endl;
      struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
      setitimer(ITIMER_PROF, &timer, NULL);
#endif
      const Plan* plan =
	Plan::plan(problem, params,
		   !ALWAYS_DELETE_ALL && pi == Problem::end());
      if (plan != NULL) {
	if (plan->complete()) {
	  if (verbosity > 0) {
#ifdef DEBUG
	    std::cerr << "Depth of solution: " << plan->depth() << std::endl;
#endif
	    std::cerr << "Number of steps: " << plan->num_steps() << std::endl;
	  }
	  std::cout << *plan << std::endl;
	} else {
	  std::cout << "no plan" << std::endl;
	  std::cout << ";Search limit reached." << std::endl;
	}
      } else {
	std::cout << "no plan" << std::endl;
	std::cout << ";Problem has no solution." << std::endl;
      }
      if (ALWAYS_DELETE_ALL || pi != Problem::end()) {
	if (plan != NULL) {
	  delete plan;
	}
	Plan::cleanup();
      }
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      /* Planning time. */
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      std::cout << "Time: " << std::max(0, int(1000.0*t + 0.5)) << std::endl;
    }
  } catch (const Exception& e) {
    std::cerr << PACKAGE ": " << e << std::endl;
    return -1;
  } catch (...) {
    std::cerr << PACKAGE ": fatal error" << std::endl;
    return -1;
  }

  return 0;
}
