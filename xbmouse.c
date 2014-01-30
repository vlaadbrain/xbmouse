/*
 * See LICENSE file for copyright and license details.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>

#include "arg.h"

static void die(const char *errstr, ...);
static void * emallocz(size_t size);
static void run(void);
static void setcmd(int argc, char *argv[]);
static void spawn(void);
static void usage(void);

char *argv0;
static Display *dpy;
static unsigned int button = 0;
static int btype = ButtonRelease;
static char **cmd = NULL;

#define PING_TIMEOUT 300
#define LENGTH(x)	(sizeof x / sizeof x[0])

void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void *
emallocz(size_t size) {
	void *p;

	if(!(p = calloc(1, size)))
		die("xbmouse: cannot malloc\n");
	return p;
}

void
run(void) {
	for(int scr=0;scr < ScreenCount(dpy); scr++) {
		XUngrabButton(dpy, AnyButton, AnyModifier, RootWindow (dpy, scr));
	}

	for(int scr=0;scr < ScreenCount(dpy); scr++) {
		XGrabButton(dpy, button, AnyModifier, RootWindow(dpy, scr),
				False, ButtonPressMask | ButtonReleaseMask,
				GrabModeAsync, GrabModeAsync, None, None);
	}

	for(;;) {
		XEvent ev;
		XNextEvent(dpy, &ev);
		if(ev.xbutton.button == button && ev.xbutton.type == btype) {
			spawn();
		}
	}
}

void
setcmd(int argc, char *argv[]) {
	int i;

	cmd = emallocz((argc+3) * sizeof(*cmd));
	if (argc == 0)
		return;
	for(i = 0; i < argc; i++)
		cmd[i] = argv[i];

	cmd[argc] = cmd[argc+1] = NULL;
}

void
spawn(void) {
	pid_t cpid;

	cpid = fork();
	if(cpid == -1) {
		perror("xbmouse failed to fork");
		exit(EXIT_FAILURE);
	}

	if(cpid == 0) {
		if(dpy)
			close(ConnectionNumber(dpy));

		setsid();
		execvp(cmd[0], cmd);
		exit(EXIT_SUCCESS);
	} else {
		wait(NULL);
	}
}

void
usage(void) {
	die("usage: %s [-vpr] [-b <button#> command ...]\n", basename(argv0));
}

int
main(int argc, char *argv[]) {
	ARGBEGIN {
		case 'b':
			button = atoi(EARGF(usage()));
			break;
		case 'p':
			btype = ButtonPress;
			break;
		case 'r':
			btype = ButtonRelease;
			break;
		case 'v':
			die("xbmouse-"VERSION", © 2014 xbmouse engineers"
					", see LICENSE for details.\n");
		default:
			usage();
	} ARGEND;

	if(button == 0)
		usage();

	setcmd(argc, argv);

	if(!(dpy = XOpenDisplay(0)))
		die("xbmouse: cannot open display.\n");

	run();

	XCloseDisplay(dpy);

	return 0;
}

