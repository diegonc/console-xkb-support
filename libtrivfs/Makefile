#
#   Copyright (C) 1994, 1995, 1996, 1997, 1999, 2001, 2002, 2003, 2008 Free
#   Software Foundation, Inc.
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

dir := libtrivfs
makemode := library

FSSRCS= dir-link.c dir-mkdir.c dir-mkfile.c dir-lookup.c dir-readdir.c \
	dir-rename.c dir-rmdir.c dir-unlink.c file-chauthor.c \
	file-chflags.c file-chmod.c file-chown.c file-get-trans.c \
	file-get-transcntl.c file-getcontrol.c file-getfh.c \
	file-getlinknode.c file-lock.c file-set-trans.c file-statfs.c \
	file-sync.c file-syncfs.c file-set-size.c file-utimes.c file-exec.c \
	file-access.c dir-chg.c file-chg.c file-get-storage-info.c \
	file-get-fs-options.c file-reparent.c

IOSRCS=io-async-icky.c io-async.c io-duplicate.c io-map.c io-modes-get.c \
	io-modes-off.c io-modes-on.c io-modes-set.c io-owner-get.c \
	io-owner-mod.c io-pathconf.c io-read.c io-readable.c io-revoke.c \
	io-reauthenticate.c io-restrict-auth.c io-seek.c io-select.c \
	io-stat.c io-stubs.c io-write.c io-version.c io-identity.c

FSYSSRCS=fsys-getroot.c fsys-goaway.c fsys-stubs.c fsys-syncfs.c \
	fsys-forward.c fsys-set-options.c fsys-get-options.c

OTHERSRCS=demuxer.c protid-clean.c protid-dup.c cntl-create.c \
	cntl-clean.c migsupport.c times.c startup.c open.c \
	runtime-argp.c set-options.c append-args.c dyn-classes.c \
	protid-classes.c cntl-classes.c

SRCS=$(FSSRCS) $(IOSRCS) $(FSYSSRCS) $(OTHERSRCS)

MIGSTUBS=fsServer.o ioServer.o fsysServer.o fsys_replyUser.o

libname = libtrivfs
HURDLIBS = fshelp iohelp ports shouldbeinlibc
OBJS= $(sort $(subst .c,.o,$(SRCS)) $(MIGSTUBS))
LCLHDRS = trivfs.h fsmutations.h priv.h
MIGSFLAGS=-imacros $(srcdir)/fsmutations.h
MIGCOMSFLAGS = -prefix trivfs_
installhdrs := trivfs.h
mig-sheader-prefix = trivfs_
ifndef no_deps
installhdrs += $(patsubst %,trivfs_%_S.h,fs io fsys)
endif

include ../Makeconf

$(MIGSTUBS:%Server.o=%.sdefsi): $(srcdir)/fsmutations.h
