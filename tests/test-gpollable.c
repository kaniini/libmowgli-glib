/*
 * test-gpollable.c
 * Test gpollable eventloop integration.
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

#include <mowgli-glib.h>

GMainLoop *mainloop;
mowgli_eventloop_t *eventloop;

static void timer_tick_glib(gpointer unused)
{
	static gint counter = 0;

	g_print("timer ticked %d times\n", ++counter);

	if (counter == 5)
		g_main_loop_quit(mainloop);
}

static void test_base_loop(void)
{
	mowgli_eventloop_timer_t *timer;

	timer = mowgli_timer_add(eventloop, "timer_tick_glib", timer_tick_glib, NULL, 1);

	g_main_loop_run(mainloop);

	mowgli_timer_destroy(eventloop, timer);
}

static void timer_tick_mowgli(gpointer data)
{
	mowgli_eventloop_t *eventloop = (mowgli_eventloop_t *) data;
	static gint counter = 0;

	g_print("timer ticked %d times\n", ++counter);

	if (counter == 5)
		mowgli_eventloop_break(eventloop);
}

static void test_mowgli_loop(void)
{
	mowgli_eventloop_timer_t *timer;

	timer = mowgli_timer_add(eventloop, "timer_tick_mowgli", timer_tick_mowgli, eventloop, 1);

	g_main_loop_run(mainloop);

	mowgli_timer_destroy(eventloop, timer);
}

int main(int argc, const char *argv[])
{
	mainloop = g_main_loop_new(NULL, FALSE);

	mowgli_glib_init(mainloop, NULL);
	eventloop = mowgli_glib_get_eventloop(mainloop);

	printf("test glib driven loop\n");
	test_base_loop();

	printf("test mowgli eventloop manipulation functions\n");
	test_mowgli_loop();
}

