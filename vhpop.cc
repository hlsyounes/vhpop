/*
 * Main program.
 *
 * $Id: vhpop.cc,v 1.1 2001-05-03 15:41:34 lorens Exp $
 */
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#define _GNU_SOURCE
#include <getopt.h>
#include "domains.h"
#include "problems.h"
#include "plans.h"
#include "heuristics.h"


/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;


/* Program name. */
static const string PROGRAM_NAME = "tpop";

/* Program options. */
static struct option long_options[] = {
  { "heuristic", required_argument, NULL, 'h' },
  { "early", optional_argument, NULL, 'e' },
  { "trans", no_argument, NULL, 't' },
  { "limit", required_argument, NULL, 'l' },
  { "verbose", optional_argument, NULL, 'v' },
  { "help", no_argument, NULL, 0 },
  { "version", no_argument, NULL, 1 },
  { 0, 0, 0, 0 }
};


/* Displays help. */
static void display_help() {
  cout << PROGRAM_NAME << endl
       << "usage: " << PROGRAM_NAME << " [options] [file ...]" << endl
       << "options:" << endl
       << "  -h h,  --heuristic=h\t"
       << "use heuristic h;" << endl
       << "\t\t\t  h can be `MAX' (default), `SUM', `SUMMAX', or `UCPOP'"
       << endl
       << "  -e[n], --early[=n]\t"
       << "enable early linking of open conditions" << endl
       << "  -t,    --trans\t"
       << "enable transformational plan operators" << endl
       << "  -l l,  --limit=l\t"
       << "search no more than l plans" << endl
       << "  -v[n], --verbose[=n]\t"
       << "use verbosity level n;" << endl
       << "\t\t\t  n is a number from 0 (verbose mode off) and up;" << endl
       << "\t\t\t  default level is 1 if optional argument is left out" << endl
       << "          --help\t"
       << "display this help and exit" << endl
       << "          --version\t"
       << "display version information and exit" << endl
       << "  file ...\t\t"
       << "files containing domain and problem descriptions;" << endl
       << "\t\t\t  if none given, use standard input" << endl;
}


/* Displays version information. */
static void display_version() {
  cout << PROGRAM_NAME << endl
       << " Written by Hakan Younes (lorens@cs.cmu.edu)." << endl;
}


/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  yyin = fopen(name, "r");
  if (yyin == NULL) {
    perror("fopen");
    return false;
  } else {
    bool success = (yyparse() == 0);
    fclose(yyin);
    return success;
  }
}


/* The main program. */
int main(int argc, char* argv[]) {
  /* Set default heuristic. */
  Heuristic heuristic = MAX_HEURISTIC;
  /* Whether to allow early linking. */
  unsigned int early_linking = 0;
  /* Whether to allow transformational plan operators. */
  bool transformations = false;
  /* Search limit. */
  unsigned int limit = 2000;
  /* Set default verbosity. */
  unsigned int verbosity = 0;

  /*
   * Get command line options.
   */
  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "h:e::tl:v::",
			long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 0:
      display_help();
      return 0;
    case 1:
      display_version();
      return 0;
    case 'h':
      if (strcasecmp(optarg, "MAX") == 0) {
	heuristic = MAX_HEURISTIC;
      } else if (strcasecmp(optarg, "SUM") == 0) {
	heuristic = SUM_HEURISTIC;
      } else if (strcasecmp(optarg, "SUMMAX") == 0) {
	heuristic = SUMMAX_HEURISTIC;
      } else if (strcasecmp(optarg, "UCPOP") == 0) {
	heuristic = UCPOP_HEURISTIC;
      } else {
	cerr << PROGRAM_NAME << ": invalid heuristic `" << optarg << "'"
	     << endl
	     << "Try `" << PROGRAM_NAME << " --help' for more information."
	     << endl;
	return -1;
      }
      break;
    case 'e':
      early_linking = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case 't':
      transformations = true;
      break;
    case 'l':
      limit = atoi(optarg);
      break;
    case 'v':
      verbosity = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case ':':
    case '?':
    default:
      cerr << "Try `" << PROGRAM_NAME << " --help' for more information."
	   << endl;
      return -1;
    }
  }

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

  if (verbosity > 0) {
    /*
     * Display domains and problems.
     */
    cout << "----------------------------------------"<< endl;
    cout << "domains:" << endl;
    for (Domain::DomainMap::const_iterator i = Domain::begin();
	 i != Domain::end(); i++) {
      cout << endl << *(*i).second << endl;
    }
    cout << "----------------------------------------"<< endl;
    cout << "problems:" << endl;
    for (Problem::ProblemMap::const_iterator i = Problem::begin();
	 i != Problem::end(); i++) {
      cout << endl << *(*i).second << endl;
    }
    cout << "----------------------------------------"<< endl;
  }

  /* Set output format for floating point numbers. */
  cout.setf(ios::fixed, ios::floatfield);
  cout.precision(3);

  /*
   * Solve the problems.
   */
  for (Problem::ProblemMap::const_iterator i = Problem::begin();
       i != Problem::end(); i++) {
    struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
    const Problem& problem = *(*i).second;
    setitimer(ITIMER_PROF, &timer, NULL);
    const Plan* plan = Plan::plan(problem, heuristic, early_linking,
				  transformations, limit, verbosity);
    getitimer(ITIMER_PROF, &timer);
    /* Planning time. */
    double t = 1000000.9
      - (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
    if (t < 0.0) {
      t = 0.0;
    }
    if (plan != NULL) {
      if (plan->complete()) {
	if (verbosity == 0) {
	  cout << problem.name << ' ' << t << ' ' << *plan << endl;
	} else {
	  cout << endl << *plan << endl;
	}
      } else {
	if (verbosity > 0) {
	  cout << endl << "Search limit reached." << endl;
	}
      }
    } else if (verbosity > 0) {
      cout << endl << "Problem has no solution." << endl;
    }
  }

  return 0;
}
