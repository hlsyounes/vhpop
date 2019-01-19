// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Main program.

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

#include "debug.h"
#include "domains.h"
#include "heuristics.h"
#include "parameters.h"
#include "plans.h"
#include "problems.h"

#include "src/timer.h"

#if HAVE_GETOPT_LONG
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#endif

/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;

/* Name of current file. */
std::string current_file;
/* Level of warnings. */
int warning_level;
/* Verbosity level. */
int verbosity;


/* Program options. */
static struct option long_options[] = {
  { "action-cost", required_argument, NULL, 'a' },
  { "domain-constraints", optional_argument, NULL, 'd' },
  { "flaw-order", required_argument, NULL, 'f' },
  { "ground-actions", no_argument, NULL, 'g' },
  { "help", no_argument, NULL, 'H' },
  { "heuristic", required_argument, NULL, 'h' },
  { "limit", required_argument, NULL, 'l' },
  { "random-open-conditions", no_argument, NULL, 'r' },
  { "search-algorithm", required_argument, NULL, 's' },
  { "seed", required_argument, NULL, 'S' },
  { "time-limit", required_argument, NULL, 'T' },
  { "tolerance", required_argument, NULL, 't' },
  { "version", no_argument, NULL, 'V' },
  { "verbose", optional_argument, NULL, 'v' },
  { "warnings", optional_argument, NULL, 'W' },
  { "weight", required_argument, NULL, 'w' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "a:d::f:gHh:l:rS:s:T:t:Vv::W::w:";


/* Displays help. */
static void display_help() {
  std::cout << "usage: " << PACKAGE << " [options] [file ...]" << std::endl
	    << "options:" << std::endl
	    << "  -a a,  --action-cost=a" << std::endl
	    << "\t\t\tuse action cost a" << std::endl
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
	    << "  -H     --help\t\t"
	    << "display this help and exit" << std::endl
	    << "  -h h,  --heuristic=h\t"
	    << "use heuristic h to rank plans" << std::endl
	    << "  -l l,  --limit=l\t"
	    << "search no more than l plans" << std::endl
	    << "  -r,    --random-open-conditions" << std::endl
	    << "\t\t\tadd open conditions in random order"
	    << std::endl
	    << "  -S s,  --seed=s\t"
	    << "uses s as seed for random number generator" << std::endl
	    << "  -s s,  --search-algorithm=s" << std::endl
	    << "\t\t\tuse search algorithm s" << std::endl
	    << "  -T t,  --time-limit=t\t"
	    << "limit search to t minutes" << std::endl
	    << "  -t t,  --tolerance=t\t"
	    << "use tolerance t with durative actions;" << std::endl
	    << "\t\t\t  time stamps less than t appart are considered"
	    << std::endl
	    << "\t\t\t  indistinguishable (default is 0.01)" << std::endl
	    << "  -v[n], --verbose[=n]\t"
	    << "use verbosity level n;" << std::endl
	    << "\t\t\t  n is a number from 0 (verbose mode off) and up;"
	    << std::endl
	    << "\t\t\t  default level is 1 if optional argument is left out"
	    << std::endl
	    << "  -V,    --version\t"
	    << "display version information and exit" << std::endl
	    << "  -W[n], --warnings[=n]\t"
	    << "determines how warnings are treated;" << std::endl
	    << "\t\t\t  0 supresses warnings; 1 displays warnings;"
	    << std::endl
	    << "\t\t\t  2 treats warnings as errors" << std::endl
	    << "  -w,    --weight=w\t"
	    << "weight to use with heuristic (default is 1)" << std::endl
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
	    << "Copyright (C) 2002--2005 Carnegie Mellon University"
            << std::endl << "Copyright (C) 2019 Google Inc" << std::endl
	    << PACKAGE_NAME
	    << " comes with NO WARRANTY, to the extent permitted by law."
	    << std::endl
	    << "For information about the terms of redistribution,"
	    << std::endl
	    << "see the file named COPYING in the " PACKAGE_NAME
	    << " distribution." << std::endl
	    << std::endl
	    << "Written by Haakan Younes." << std::endl;
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
}


/* The main program. */
int main(int argc, char* argv[]) {
  atexit(cleanup);

  const bool free_all_memory = getenv("VHPOP_FREE_ALL_MEMORY");
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
#if HAVE_GETOPT_LONG
    int c = getopt_long(argc, argv, OPTION_STRING, long_options, &option_index);
#else
    int c = getopt(argc, argv, OPTION_STRING);
#endif
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'a':
      try {
	params.set_action_cost(optarg);
      } catch (const InvalidActionCost& e) {
	std::cerr << PACKAGE ": " << e.what() << std::endl
		  << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return -1;
      }
      break;
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
	std::cerr << PACKAGE << ": " << e.what() << std::endl
	     << "Try `" << PACKAGE << " --help' for more information."
	     << std::endl;
	return -1;
      }
      break;
    case 'g':
      params.ground_actions = true;
      break;
    case 'H':
      display_help();
      return 0;
    case 'h':
      try {
	params.heuristic = optarg;
      } catch (const InvalidHeuristic& e) {
	std::cerr << PACKAGE ": " << e.what() << std::endl
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
	params.search_limits.push_back(std::numeric_limits<unsigned int>::max());
      } else {
	params.search_limits.push_back(atoi(optarg));
      }
      break;
    case 'r':
      params.random_open_conditions = true;
      break;
    case 'S':
      srand(atoi(optarg));
      break;
    case 's':
      try {
	params.set_search_algorithm(optarg);
      } catch (const InvalidSearchAlgorithm& e) {
	std::cerr << PACKAGE ": " << e.what() << std::endl
		  << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return -1;
      }
      break;
    case 'T':
      params.time_limit = std::chrono::minutes(atoi(optarg));
      break;
    case 't':
      if (optarg == std::string("unlimited")) {
	Orderings::threshold = std::numeric_limits<unsigned int>::max();
      } else {
	Orderings::threshold = atof(optarg);
      }
      break;
    case 'V':
      display_version();
      return 0;
    case 'v':
      verbosity = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case 'W':
      warning_level = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case 'w':
      params.weight = atof(optarg);
      break;
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
      for (Domain::DomainMap::const_iterator di = Domain::begin();
	   di != Domain::end(); di++) {
	std::cerr << std::endl << *(*di).second << std::endl;
      }
      std::cerr << "----------------------------------------"<< std::endl
		<< "problems:" << std::endl;
      for (Problem::ProblemMap::const_iterator pi = Problem::begin();
	   pi != Problem::end(); pi++) {
	std::cerr << std::endl << *(*pi).second << std::endl;
      }
      std::cerr << "----------------------------------------"<< std::endl;
    }

    std::cerr.setf(std::ios::unitbuf);

    /*
     * Solve the problems.
     */
    for (Problem::ProblemMap::const_iterator pi = Problem::begin();
	 pi != Problem::end(); ) {
      const Problem& problem = *(*pi).second;
      pi++;
      std::cout << ';' << problem.name() << std::endl;
      Timer<> timer;
      const Plan* plan =
          Plan::plan(problem, params, !free_all_memory && pi == Problem::end());
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
      if (free_all_memory || pi != Problem::end()) {
        if (plan != NULL) {
	  delete plan;
	}
	Plan::cleanup();
      }
      /* Planning time. */
      const auto elapsed_millis =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              timer.ElapsedTime());
      std::cout << "Time: " << elapsed_millis.count() << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << PACKAGE ": " << e.what() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << PACKAGE ": fatal error" << std::endl;
    return -1;
  }

  return 0;
}
