/* Copyright (C) 2009  Pierre-Marc Fournier
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <fcntl.h>

#include "ustcomm.h"
#include "ustcmd.h"
#include "usterr.h"

enum command {
	START_TRACE,
	STOP_TRACE,
	START,
	DESTROY,
	LIST_MARKERS,
	ENABLE_MARKER,
	DISABLE_MARKER,
	GET_ONLINE_PIDS,
	UNKNOWN
};

struct ust_opts {
	enum command cmd;
	pid_t *pids;
	char *regex;
};

char *progname = NULL;

void usage(void)
{
	fprintf(stderr, "usage: %s COMMAND PIDs...\n", progname);
	fprintf(stderr, "\nControl the tracing of a process that supports LTTng Userspace Tracing.\n\
\n\
Commands:\n\
    --start-trace\t\t\tStart tracing\n\
    --stop-trace\t\t\tStop tracing\n\
    --destroy-trace\t\t\tDestroy the trace\n\
    --enable-marker \"CHANNEL/MARKER\"\tEnable a marker\n\
    --disable-marker \"CHANNEL/MARKER\"\tDisable a marker\n\
    --list-markers\t\t\tList the markers of the process, their\n\t\t\t\t\t  state and format string\n\
\
");
}

int parse_opts_long(int argc, char **argv, struct ust_opts *opts)
{
	int c;

	opts->pids = NULL;
	opts->regex = NULL;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"start-trace", 0, 0, 1000},
			{"stop-trace", 0, 0, 1001},
			{"destroy-trace", 0, 0, 1002},
			{"list-markers", 0, 0, 1004},
			{"print-markers", 0, 0, 1005},
			{"pid", 1, 0, 1006},
			{"enable-marker", 1, 0, 1007},
			{"disable-marker", 1, 0, 1008},
			{"start", 0, 0, 1009},
			{"help", 0, 0, 'h'},
			{"version", 0, 0, 1010},
			{"online-pids", 0, 0, 1011},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "h", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;

		case 1000:
			opts->cmd = START_TRACE;
			break;
		case 1001:
			opts->cmd = STOP_TRACE;
			break;
		case 1009:
			opts->cmd = START;
			break;
		case 1002:
			opts->cmd = DESTROY;
			break;
		case 1004:
			opts->cmd = LIST_MARKERS;
			break;
		case 1007:
			opts->cmd = ENABLE_MARKER;
			opts->regex = strdup(optarg);
			break;
		case 1008:
			opts->cmd = DISABLE_MARKER;
			opts->regex = strdup(optarg);
			break;
		case 1011:
			opts->cmd = GET_ONLINE_PIDS;
			break;
		case 'h':
			usage();
			exit(0);
		case 1010:
			printf("Version 0.1\n");

		default:
			/* unknown option or other error; error is
			printed by getopt, just return */
			opts->cmd = UNKNOWN;
			return 1;
		}
	}

	if (argc - optind > 0 && opts->cmd != GET_ONLINE_PIDS) {
		int i;
		int pididx=0;
		opts->pids = malloc((argc-optind+1) * sizeof(pid_t));

		for(i=optind; i<argc; i++) {
			/* don't take any chances, use a long long */
			long long tmp;
			char *endptr;
			tmp = strtoull(argv[i], &endptr, 10);
			if(*endptr != '\0') {
				ERR("The pid \"%s\" is invalid.", argv[i]);
				return 1;
			}
			opts->pids[pididx++] = (pid_t) tmp;
		}
		opts->pids[pididx] = -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	pid_t *pidit;
	int result;
	struct ust_opts opts;

	progname = argv[0];

	if(argc <= 1) {
		fprintf(stderr, "No operation specified.\n");
		usage();
		exit(EXIT_FAILURE);
	}

	result = parse_opts_long(argc, argv, &opts);
	if(result) {
		fprintf(stderr, "\n");
		usage();
		exit(EXIT_FAILURE);
	}

	if(opts.pids == NULL && opts.cmd != GET_ONLINE_PIDS) {
		fprintf(stderr, "No pid specified.\n");
		usage();
		exit(EXIT_FAILURE);
	}
	if(opts.cmd == UNKNOWN) {
		fprintf(stderr, "No command specified.\n");
		usage();
		exit(EXIT_FAILURE);
	}
	if (opts.cmd == GET_ONLINE_PIDS) {
		pid_t *pp = ustcmd_get_online_pids();
		unsigned int i = 0;

		if (pp) {
			while (pp[i] != 0) {
				printf("%u\n", (unsigned int) pp[i]);
				++i;
			}
			free(pp);
		}

		exit(EXIT_SUCCESS);
	}

	pidit = opts.pids;
	struct marker_status *cmsf = NULL;

	while(*pidit != -1) {
		switch (opts.cmd) {
			case START_TRACE:
				result = ustcmd_start_trace(*pidit);
				if (result) {
					ERR("error while trying to for trace with PID %u\n", (unsigned int) *pidit);
					break;
				}
				//printf("sucessfully started trace for PID %u\n", (unsigned int) *pidit);
				break;

			case STOP_TRACE:
				result = ustcmd_stop_trace(*pidit);
				if (result) {
					ERR("error while trying to stop trace for PID %u\n", (unsigned int) *pidit);
					break;
				}
				//printf("sucessfully stopped trace for PID %u\n", (unsigned int) *pidit);
				break;

			case START:
				result = ustcmd_setup_and_start(*pidit);
				if (result) {
					ERR("error while trying to setup/start trace for PID %u\n", (unsigned int) *pidit);
					break;
				}
				//printf("sucessfully setup/started trace for PID %u\n", (unsigned int) *pidit);
				break;

			case DESTROY:
				result = ustcmd_destroy_trace(*pidit);
				if (result) {
					ERR("error while trying to destroy trace with PID %u\n", (unsigned int) *pidit);
					break;
				}
				//printf("sucessfully destroyed trace for PID %u\n", (unsigned int) *pidit);
				break;

			case LIST_MARKERS:
				cmsf = NULL;
				if (ustcmd_get_cmsf(&cmsf, *pidit)) {
					fprintf(stderr,
						"error while trying to list markers for"
						" PID %u\n", (unsigned int) *pidit);
					break;
				}
				unsigned int i = 0;
				while (cmsf[i].channel != NULL) {
					printf("{PID: %u, channel/marker: %s/%s, "
						"state: %u, fmt: %s}\n",
						(unsigned int) *pidit,
						cmsf[i].channel,
						cmsf[i].marker,
						cmsf[i].state,
						cmsf[i].fs);
					++i;
				}
				ustcmd_free_cmsf(cmsf);
				break;

			case ENABLE_MARKER:
				if(opts.regex)
					ustcmd_set_marker_state(opts.regex, 1, *pidit);
				break;
			case DISABLE_MARKER:
				if(opts.regex)
					ustcmd_set_marker_state(opts.regex, 0, *pidit);
				break;

			default:
				ERR("unknown command\n");
			break;
		}

		pidit++;
	}

	if (opts.pids != NULL) {
		free(opts.pids);
	}
	if (opts.regex != NULL) {
		free(opts.regex);
	}

	return 0;
}
