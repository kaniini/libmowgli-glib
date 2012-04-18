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

static mowgli_dictionary_t *mainloop_dict = NULL;
static GPollFunc original_pollfn = NULL;

static ptrdiff_t compare_pointer(const void *lhs, const void *rhs)
{
	return rhs - lhs;
}

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
	GMainContext *context;
	guint i;
	mowgli_eventloop_pollable_t *pfds[nfds + 1];
	mowgli_eventloop_t *eventloop;

	context = g_main_context_get_thread_default();
	if (context == NULL)
		context = g_main_context_default();

	eventloop = mowgli_dictionary_retrieve(mainloop_dict, context);
	if (eventloop == NULL)
		mowgli_throw_exception_fatal(mowgli.glib.UnavailableContextException);

	for (i = 0; i < nfds; i++)
	{
		pfds[i] = mowgli_pollable_create(eventloop, pollfds[i].fd, &pollfds[i]);

		if (pollfds[i].events & G_IO_IN)
			mowgli_pollable_setselect(eventloop, pfds[i], MOWGLI_EVENTLOOP_IO_READ, mowgli_glib_poll_mark_readable);
		if (pollfds[i].events & G_IO_OUT)
			mowgli_pollable_setselect(eventloop, pfds[i], MOWGLI_EVENTLOOP_IO_WRITE, mowgli_glib_poll_mark_writeable);
	}

	mowgli_eventloop_timeout_once(eventloop, timeout);

	for (i = 0; i < nfds; i++)
		mowgli_pollable_destroy(eventloop, pfds[i]);

	/* allow breaking the GMainLoop using mowgli_eventloop_break() */
	if (eventloop->death_requested)
	{
		GMainLoop *mainloop = mowgli_eventloop_get_data(eventloop);
		g_main_loop_quit(mainloop);
	}

	return nfds;
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

	if (eventloop == NULL)
		eventloop = mowgli_eventloop_create();

	/* we have to do this because glib doesn't allow you to attach data to a GMainContext */
	if (mainloop_dict == NULL)
		mainloop_dict = mowgli_dictionary_create(compare_pointer);

	mowgli_dictionary_add(mainloop_dict, main_context, eventloop);

	/* associate our GMainLoop with our eventloop. */
	mowgli_eventloop_set_data(eventloop, mainloop);
}

mowgli_eventloop_t *
mowgli_glib_get_eventloop(GMainLoop *mainloop)
{
	GMainContext *main_context;

	return_val_if_fail(mainloop != NULL, NULL);

	main_context = g_main_loop_get_context(mainloop);
	return_val_if_fail(main_context != NULL, NULL);

	/* we have to do this because glib doesn't allow you to attach data to a GMainContext */
	if (mainloop_dict == NULL)
		mainloop_dict = mowgli_dictionary_create(compare_pointer);

	return mowgli_dictionary_retrieve(mainloop_dict, main_context);
}

mowgli_eventloop_t *
mowgli_glib_get_eventloop_default(void)
{
	GMainContext *context;

	context = g_main_context_get_thread_default();
	if (context == NULL)
		context = g_main_context_default();

	/* we have to do this because glib doesn't allow you to attach data to a GMainContext */
	if (mainloop_dict == NULL)
		mainloop_dict = mowgli_dictionary_create(compare_pointer);

	return mowgli_dictionary_retrieve(mainloop_dict, context);
}
