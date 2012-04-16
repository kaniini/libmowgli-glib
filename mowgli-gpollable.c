/*
 * mowgli-gpollable.c
 * Integration for GPollFD objects into Mowgli eventloops.
 *
 * Copyright (c) 2012 William Pitcock <nenolod@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "mowgli-glib.h"
#include "config.h"

static mowgli_eventloop_t *base_eventloop = NULL;
static GPollFunc original_pollfn = NULL;

/*
 * This kind of sucks, but it's better than nothing, I suppose.
 * The good news is GLib probably won't use many FDs.
 */
static void
mowgli_glib_poll_mark_readable(mowgli_eventloop_t *eventloop, mowgli_eventloop_io_t *io, mowgli_eventloop_io_dir_t dir, void *userdata)
{
	GPollFD *pollfd = userdata;

	pollfd->revents |= G_IO_IN;
}

static void
mowgli_glib_poll_mark_writeable(mowgli_eventloop_t *eventloop, mowgli_eventloop_io_t *io, mowgli_eventloop_io_dir_t dir, void *userdata)
{
	GPollFD *pollfd = userdata;

	pollfd->revents |= G_IO_OUT;
}

static gint
mowgli_glib_poll(GPollFD *pollfds, guint nfds, gint timeout)
{
	guint i;
	mowgli_eventloop_pollable_t *pfds[nfds + 1];

	for (i = 0; i < nfds; i++)
	{
		pfds[i] = mowgli_pollable_create(base_eventloop, pollfds[i].fd, &pollfds[i]);

		if (pollfds[i].events & G_IO_IN)
			mowgli_pollable_setselect(base_eventloop, pfds[i], MOWGLI_EVENTLOOP_IO_READ, mowgli_glib_poll_mark_readable);
		if (pollfds[i].events & G_IO_OUT)
			mowgli_pollable_setselect(base_eventloop, pfds[i], MOWGLI_EVENTLOOP_IO_WRITE, mowgli_glib_poll_mark_writeable);
	}

	mowgli_eventloop_timeout_once(base_eventloop, timeout);

	for (i = 0; i < nfds; i++)
		mowgli_pollable_destroy(base_eventloop, pfds[i]);
}

gboolean
mowgli_glib_init(GMainLoop *mainloop, mowgli_eventloop_t *eventloop)
{
	GMainContext *main_context;

	return_val_if_fail(mainloop != NULL, FALSE);

	main_context = g_main_loop_get_context(mainloop);
	return_val_if_fail(main_context != NULL, FALSE);

	original_pollfn = g_main_context_get_poll_func(main_context);
	g_main_context_set_poll_func(main_context, mowgli_glib_poll);

	if (eventloop)
		base_eventloop = eventloop;
	else
		base_eventloop = mowgli_eventloop_create();
}

mowgli_eventloop_t *
mowgli_glib_get_eventloop(GMainLoop *mainloop)
{
	return base_eventloop;
}
