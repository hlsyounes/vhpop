/*
 * Main program.
 *
 * $Id: vhpop.cc,v 1.10 2001-09-18 16:22:35 lorens Exp $
 */
#include <iostream>
#include <cstdio>
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
/* Name of current file. */
extern string current_file;
/* Level of warnings. */
extern int warning_level;

/* Program name. */
string PROGRAM_NAME("tpop");

/* Program options. */
static struct option long_options[] = {
  { "flaw-order", required_argument, NULL, 'f' },
  { "ground", no_argument, NULL, 'g' },
  { "heuristic", required_argument, NULL, 'h' },
  { "limit", required_argument, NULL, 'l' },
#ifdef TRANSFORMATIONAL
  { "trans", no_argument, NULL, 't' },
#endif
  { "verbose", optional_argument, NULL, 'v' },
  { "version", no_argument, NULL, 'V' },
  { "warnings", optional_argument, NULL, 'w' },
  { "help", no_argument, NULL, '?' },
  { 0, 0, 0, 0 }
};


/* Displays help. */
static void display_help() {
  cout << PROGRAM_NAME << endl
       << "usage: " << PROGRAM_NAME << " [options] [file ...]" << endl
       << "options:" << endl
       << "  -f f,  --flaw-order=f\t"
       << "use flaw selection order f;" << endl
       << "\t\t\t  f can be `LIFO' (default), `FIFO', `MC', `LC'," << endl
       << "\t\t\t  `MW', `LW', `ML', or `LL'"
       << endl
       << "  -g,    --ground\t"
       << "only use ground actions" << endl
       << "  -h h,  --heuristic=h\t"
       << "use heuristic h;" << endl
       << "\t\t\t  h can be `MAX', `SUM', `SUMR' (default), or `UCPOP'" << endl
       << "  -l l,  --limit=l\t"
       << "search no more than l plans" << endl
#ifdef TRANSFORMATIONAL
       << "  -t,    --trans\t"
       << "enable transformational plan operators" << endl
#endif
       << "  -v[n], --verbose[=n]\t"
       << "use verbosity level n;" << endl
       << "\t\t\t  n is a number from 0 (verbose mode off) and up;" << endl
       << "\t\t\t  default level is 1 if optional argument is left out" << endl
       << "  -V     --version\t"
       << "display version information and exit" << endl
       << "  -w[n], --warnings[=n]\t"
       << "determines how warnings are treated;" << endl
       << "\t\t\t  0 supresses warnings; 1 displays warnings;" << endl
       << "\t\t\t  2 treats warnings as errors" << endl
       << "  -?     --help\t\t"
       << "display this help and exit" << endl
       << "  file ...\t\t"
       << "files containing domain and problem descriptions;" << endl
       << "\t\t\t  if none given, use standard input" << endl;
}


/* Displays version information. */
static void display_version() {
  cout << PROGRAM_NAME << " (" << __DATE__ << ")" << endl
       << "Written by H\345kan L. Younes (lorens@cs.cmu.edu)." << endl;
}


/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  current_file = name;
  yyin = fopen(name, "r");
  if (yyin == NULL) {
    perror(PROGRAM_NAME.c_str());
    return false;
  } else {
    bool success = (yyparse() == 0);
    fclose(yyin);
    return success;
  }
}


/* The main program. */
int main(int argc, char* argv[]) {
  /* Flaw selection order. */
  FlawSelectionOrder flaw_order;
  /* Set default heuristic. */
  Heuristic heuristic;
  /* Whether to just use ground actions. */
  bool ground_actions = false;
  /* Whether to allow transformational plan operators. */
  bool transformations = false;
  /* Search limit. */
  size_t limit = 2000;
  /* Set default verbosity. */
  int verbosity = 0;
  /* Set default warning level. */
  warning_level = 1;

  /*
   * Get command line options.
   */
  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "f:gh:l:tv::Vw::?",
			long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'f':
      if (strcasecmp(optarg, "LIFO") == 0) {
	flaw_order.set_lifo();
      } else if (strcasecmp(optarg, "FIFO") == 0) {
	flaw_order.set_fifo();
      } else if (strcasecmp(optarg, "MC") == 0) {
	flaw_order.set_most_cost_first();
      } else if (strcasecmp(optarg, "LC") == 0) {
	flaw_order.set_least_cost_first();
      } else if (strcasecmp(optarg, "MW") == 0) {
	flaw_order.set_most_work_first();
      } else if (strcasecmp(optarg, "LW") == 0) {
	flaw_order.set_least_work_first();
      } else if (strcasecmp(optarg, "ML") == 0) {
	flaw_order.set_most_linkable_first();
      } else if (strcasecmp(optarg, "LL") == 0) {
	flaw_order.set_least_linkable_first();
      } else {
	cerr << PROGRAM_NAME << ": invalid flaw selection order `" << optarg
	     << "'" << endl
	     << "Try `" << PROGRAM_NAME << " --help' for more information."
	     << endl;
	return -1;
      }
      break;
    case 'g':
      ground_actions = true;
      break;
    case 'h':
      if (strcasecmp(optarg, "MAX") == 0) {
	heuristic.set_max();
      } else if (strcasecmp(optarg, "SUM") == 0) {
	heuristic.set_sum();
      } else if (strcasecmp(optarg, "SUMR") == 0) {
	heuristic.set_sum_reuse();
      } else if (strcasecmp(optarg, "UCPOP") == 0) {
	heuristic.set_ucpop();
      } else {
	cerr << PROGRAM_NAME << ": invalid heuristic `" << optarg << "'"
	     << endl
	     << "Try `" << PROGRAM_NAME << " --help' for more information."
	     << endl;
	return -1;
      }
      break;
    case 'l':
      limit = atoi(optarg);
      break;
    case 't':
      transformations = true;
      break;
    case 'v':
      verbosity = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case 'V':
      display_version();
      return 0;
    case 'w':
      warning_level = (optarg != NULL) ? atoi(optarg) : 1;
      break;
    case ':':
    case '?':
      if (optopt == '?') {
	display_help();
	return 0;
      }
    default:
      cerr << "Try `" << PROGRAM_NAME << " --help' for more information."
	   << endl;
      return -1;
    }
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
      const Plan* plan = Plan::plan(problem, flaw_order, heuristic,
				    ground_actions, transformations, limit,
				    verbosity);
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
	    cout << endl << *plan;
	    cout << endl << "Search limit reached." << endl;
	  }
	}
      } else if (verbosity > 0) {
	cout << endl << "Problem has no solution." << endl;
      }
    }
  } catch (const Unimplemented& ex) {
    cerr << ex << endl;
    return -1;
  } catch (...) {
    cerr << "fatal error" << endl;
    return -1;
  }

  return 0;
}
