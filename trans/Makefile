#
#   Copyright (C) 1994, 1995, 1996, 1997, 1999, 2000, 2001, 2002, 2006, 2007,
#   2008 Free Software Foundation, Inc.
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2, or (at
#   your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

dir := trans
makemode := servers

targets = symlink firmlink ifsock magic null fifo new-fifo fwd crash \
	  password hello hello-mt streamio fakeroot proxy-defpager
SRCS = ifsock.c symlink.c magic.c null.c fifo.c new-fifo.c fwd.c \
	crash.c firmlink.c password.c hello.c hello-mt.c streamio.c \
	fakeroot.c proxy-defpager.c
OBJS = $(SRCS:.c=.o) fsysServer.o ifsockServer.o passwordServer.o \
	crashServer.o crash_replyUser.o msgServer.o \
	default_pagerServer.o default_pagerUser.o \
	device_replyServer.o elfcore.o
HURDLIBS = threads ports netfs trivfs iohelp fshelp pipe ihash shouldbeinlibc
password-LDLIBS = $(LIBCRYPT)

include ../Makeconf

vpath elfcore.c $(top_srcdir)/exec

symlink: fsysServer.o
ifsock: ifsockServer.o
crash: crashServer.o crash_replyUser.o msgServer.o elfcore.o
password: passwordServer.o
streamio: device_replyServer.o
proxy-defpager: default_pagerServer.o default_pagerUser.o

proxy-defpager crash password streamio: ../libthreads/libthreads.a ../libports/libports.a ../libtrivfs/libtrivfs.a ../libthreads/libthreads.a ../libfshelp/libfshelp.a
fifo new-fifo: ../libpipe/libpipe.a
fwd: ../libfshelp/libfshelp.a ../libports/libports.a
hello-mt magic null ifsock fifo new-fifo firmlink: ../libtrivfs/libtrivfs.a ../libfshelp/libfshelp.a ../libthreads/libthreads.a ../libports/libports.a ../libihash/libihash.a
hello: ../libtrivfs/libtrivfs.a ../libfshelp/libfshelp.a ../libports/libports.a ../libihash/libihash.a
fakeroot: ../libnetfs/libnetfs.a ../libfshelp/libfshelp.a ../libiohelp/libiohelp.a ../libthreads/libthreads.a ../libports/libports.a ../libihash/libihash.a
$(targets): ../libshouldbeinlibc/libshouldbeinlibc.a

$(targets): %: %.o
