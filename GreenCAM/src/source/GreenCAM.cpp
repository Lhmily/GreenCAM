#include "../header/stdinc.h"
#include "../header/nfa.h"
#include "../header/dfa.h"
#include "../header/hybrid_fa.h"
#include "../header/parser.h"
#include "../header/trace.h"
#include "../header/TransTable.h"
#include <iostream>

using namespace std;

#ifndef CUR_VER
#define CUR_VER		"Michela  Becchi 1.4.1"
#endif

int VERBOSE;
int DEBUG;

void handle(DFA *dfa);

/*
 * Returns the current version string
 */
void version() {
	printf("version:: %s\n", CUR_VER);
}

/* usage */
static void usage() {

	cout << endl << "Usage: GreenCAM [options]" << endl;
	cout << "             [-p <regex_file> [--m|--i]  <in_file>]" << endl;

	cout << "             [-tcam]" << endl << endl;

	cout << "\nOther:" << endl;
	cout << "    -- -p <regex_file>  process regex file" << endl;
	cout << "    --m,--i  m modifier, ignore case" << endl;
	cout << "    -tcam step    tcam" << endl << endl;
	exit(0);
}

/* configuration */
static struct conf {
	char *regex_file;
	char *in_file;
	char *out_file;
	char *dot_file;
	char *trace_file;
	bool i_mod;
	bool m_mod;
	bool verbose;
	bool debug;
	bool hfa;
	bool tcam;
} config;

/* initialize the configuration */
void init_conf() {
	config.regex_file = NULL;
	config.in_file = NULL;
	config.out_file = NULL;
	config.dot_file = NULL;
	config.trace_file = NULL;
	config.i_mod = false;
	config.m_mod = false;
	config.debug = false;
	config.verbose = false;
	config.hfa = false;
	config.tcam = false;
}

/* print the configuration */
void print_conf() {
	fprintf(stderr, "\nCONFIGURATION: \n");
	if (config.regex_file)
		fprintf(stderr, "- RegEx file: %s\n", config.regex_file);
	if (config.in_file)
		fprintf(stderr, "- DFA import file: %s\n", config.in_file);
	if (config.out_file)
		fprintf(stderr, "- DFA export file: %s\n", config.out_file);
	if (config.dot_file)
		fprintf(stderr, "- DOT file: %s\n", config.dot_file);
	if (config.trace_file)
		fprintf(stderr, "- Trace file: %s\n", config.trace_file);
	if (config.i_mod)
		fprintf(stderr, "- ignore case selected\n");
	if (config.m_mod)
		fprintf(stderr, "- m modifier selected\n");
	if (config.verbose && !config.debug)
		fprintf(stderr, "- verbose mode\n");
	if (config.debug)
		fprintf(stderr, "- debug mode\n");
	if (config.hfa)
		fprintf(stderr, "- hfa generation invoked\n");
}

/* parse the main call parameters */
static int parse_arguments(int argc, char **argv) {
	int i = 1;
	if (argc < 2) {
		usage();
		return 0;
	}
	while (i < argc) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			usage();
			return 0;
		} else if (strcmp(argv[i], "-tcam") == 0) {
			config.tcam = true;
		} else if (strcmp(argv[i], "-p") == 0) {
			i++;
			if (i == argc) {
				fprintf(stderr, "Regular expression file name missing.\n");
				return 0;
			}
			config.regex_file = argv[i];
		} else if (strcmp(argv[i], "--m") == 0) {
			config.m_mod = true;
		} else if (strcmp(argv[i], "--i") == 0) {
			config.i_mod = true;
		} else {
			fprintf(stderr, "Ignoring invalid option %s\n", argv[i]);
		}
		i++;
	}
	return 1;
}

/* check that the given file can be read/written */
void check_file(char *filename, char *mode) {
	FILE *file = fopen(filename, mode);
	if (file == NULL) {
		fprintf(stderr, "Unable to open file %s in %c mode", filename, mode);
		fatal("\n");
	} else
		fclose(file);
}

/*
 *  MAIN - entry point
 */
int main(int argc, char **argv) {

	//read configuration
	init_conf();
	while (!parse_arguments(argc, argv))
		usage();
	print_conf();
	VERBOSE = config.verbose;
	DEBUG = config.debug;
	if (DEBUG)
		VERBOSE = 1;

	//check that it is possible to open the files
	if (config.regex_file != NULL)
		check_file(config.regex_file, "r");
	if (config.in_file != NULL)
		check_file(config.in_file, "r");
	if (config.out_file != NULL)
		check_file(config.out_file, "w");
	if (config.dot_file != NULL)
		check_file(config.dot_file, "w");
	if (config.trace_file != NULL)
		check_file(config.trace_file, "r");

	// check that either a regex file or a DFA import file are given as input
	if (config.regex_file == NULL && config.in_file == NULL) {
		fatal(
				"No data file - please use either a regex or a DFA import file\n");
	}
	if (config.regex_file != NULL && config.in_file != NULL) {
		printf(
				"DFA will be imported from the Regex file. Import file will be ignored");
	}

	/* FA declaration */
	NFA *nfa = NULL;  	// NFA
	DFA *dfa = NULL;		// DFA
	dfa_set *dfas = NULL; // set of DFAs, in case a single DFA for all RegEx in the set is not possible
	HybridFA *hfa = NULL; // Hybrid-FA

	// if regex file is provided, parses it and instantiate the corresponding NFA.
	// if feasible, convert the NFA to DFA
	if (config.regex_file != NULL) {
		FILE *regex_file = fopen(config.regex_file, "r");
		fprintf(stderr, "\nParsing the regular expression file %s ...\n",
				config.regex_file);
		regex_parser *parse = new regex_parser(config.i_mod, config.m_mod);
		nfa = parse->parse(regex_file);
		nfa->remove_epsilon();
		nfa->reduce();
		dfa = nfa->nfa2dfa();
		if (dfa == NULL)
			printf(
					"Max DFA size %ld exceeded during creation: the DFA was not generated\n",
					MAX_DFA_SIZE);
		else
			dfa->minimize();
		fclose(regex_file);
		delete parse;
	}

	handle(dfa);

	// if a regex file is not provided, import the DFA
	if (config.regex_file == NULL && config.in_file != NULL) {
		FILE *in_file = fopen(config.in_file, "r");
		fprintf(stderr, "\nImporting from file %s ...\n", config.in_file);
		dfa = new DFA();
		dfa->get(in_file);
		fclose(in_file);
	}

	// DFA export
	if (dfa != NULL && config.out_file != NULL) {
		FILE *out_file = fopen(config.out_file, "w");
		fprintf(stderr, "\nExporting to file %s ...\n", config.out_file);
		dfa->put(out_file);
		fclose(out_file);
	}

	// DOT file generation
	if (dfa != NULL && config.dot_file != NULL) {
		FILE *dot_file = fopen(config.dot_file, "w");
		fprintf(stderr, "\nExporting to DOT file %s ...\n", config.dot_file);
		char string[100];
		if (config.regex_file != NULL)
			sprintf(string, "source: %s", config.regex_file);
		else
			sprintf(string, "source: %s", config.in_file);
		dfa->to_dot(dot_file, string);
		fclose(dot_file);
	}

	// HFA generation
	if (config.hfa) {
		if (nfa == NULL)
			fatal("Impossible to build a Hybrid-FA if no NFA is given.");
		hfa = new HybridFA(nfa);
		if (hfa->get_head()->size() < 100000)
			hfa->minimize();
		printf(
				"HFA:: head size=%d, tail size=%d, number of tails=%d, border size=%d\n",
				hfa->get_head()->size(), hfa->get_tail_size(),
				hfa->get_num_tails(), hfa->get_border()->size());
	}

	// trace file traversal
	if (config.trace_file) {
		trace *tr = new trace(config.trace_file);
		if (nfa != NULL)
			tr->traverse(nfa);
		if (dfa != NULL) {
			tr->traverse(dfa);
			if (dfa->get_default_tx() != NULL)
				tr->traverse_compressed(dfa);
		}
		if (hfa != NULL)
			tr->traverse(hfa);
		delete tr;
	}

	// if the DFA was not generated because of state blow-up during NFA-DFA transformation,
	// then generate multiple DFAs
	if (config.regex_file != NULL && dfa == NULL) {
		printf("\nCreating multiple DFAs...\n");
		FILE *re_file = fopen(config.regex_file, "r");
		regex_parser *parser = new regex_parser(config.i_mod, config.m_mod);
		dfas = parser->parse_to_dfa(re_file);
		printf("%d DFAs created\n", dfas->size());
		fclose(re_file);
		delete parser;
		int idx = 0;
		FOREACH_DFASET(dfas,it){
		printf("DFA #%d::  size=%ld\n",idx,(*it)->size());
		if (config.out_file!=NULL) {
			char out_file[100];
			sprintf(out_file,"%s%d",config.out_file,idx);
			FILE *file=fopen(out_file,"w");
			fprintf(stderr,"Exporting DFA #%d to file %s ...\n",idx,out_file);
			(*it)->put(file);
			fclose(file);
			idx++;
		}
	}
}

/*
 * ADD YOUR CODE HERE
 * This is the right place to call the compression algorithms (see dfa.h, nfa.h),
 * and analyze your data structures.
 */

/* BEGIN USER CODE */

// write your code here
/* END USER CODE */

/* Automata de-allocation */

	if (nfa != NULL)
		delete nfa;
	if (dfa != NULL)
		delete dfa;
	if (dfas != NULL) {
		FOREACH_DFASET(dfas,it)delete (*it);
		delete dfas;
	}
	if (hfa != NULL)
		delete hfa;

	return 0;

}
void handle(DFA *dfa) {
	if (NULL == dfa || !config.tcam)
		return;
	char string[100];
	if (config.regex_file != NULL)
		sprintf(string, "source: %s", config.regex_file);
	printf("\nHandling D2FA...");
	dfa->D2FA(10000, true);
	dfa->merg_tree_root();

	TransTable transtable(dfa);
	transtable.build_encode_tree();
	transtable.character_compress();
	transtable.print_tree_dot();
	transtable.prefix_compress();

	transtable.print_compress_table();
	transtable.print_rebuild_tree_dot();
	transtable.print();
}

