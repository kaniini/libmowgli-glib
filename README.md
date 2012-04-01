### libmowgli-glib

Libmowgli-glib is a bridge library which allows you to use both GLib and Mowgli
eventing primitives (and thusly, higher-level eventing constructs) cooperatively.

In effect, this is accomplished by translating GLib eventing setups into Mowgli
events, and then letting Mowgli's high performance eventloop handle them.

Unfortunately, due to present limitations in the way GLib handles eventing primitives,
this library is not as efficient as it could be, which means GLib eventing will
still be slightly slower than the native Mowgli eventing constructs.

This could be solved by teaching GLib how to effectively use edge-triggered
eventing sources.  Such a change would reduce the complexity of the bridge code
so that it would simply be an O(1) translation.

### Installation

Standard ./configure; make; make install.

Like other packages using [buildsys](http://github.com/Midar/buildsys), we support
DESTDIR for packagers.

### Using

If you are adding Mowgli eventing to an already existing GLib application, use the
following example code as a general idea of how you should go about doing it:

```C

#include <mowgli-glib.h>
#include <glib.h>

int main(int argc, const char *argv[])
{
	GMainLoop *mainloop;

	mainloop = g_main_loop_new();
	mowgli_glib_init(mainloop);

	g_main_loop_run(mainloop);
}

```

You can then attach Mowgli eventing primitives to it, like so:

```C

static void timer_tick(gpointer unused)
{
	printf("timer ticked\n");
}

int main(int argc, const char *argv[])
{
	GMainLoop *mainloop;
	mowgli_eventloop_t *eventloop;

	mainloop = g_main_loop_new();

	mowgli_glib_init(mainloop);
	eventloop = mowgli_glib_get_eventloop(mainloop);

	mowgli_timer_add(eventloop, "timer_tick", timer_tick, NULL, 1);

	g_main_loop_run(mainloop);
}

```

This will cause timer_tick() to be called every second.  You can do the same thing
with pollables, VIO objects, etc.

The other way is equally simple, instead of using mowgli_eventloop_run(), use
g_main_loop_run() or some other similar wrapper after injecting mowgli into the
eventloop.
