/* Zero store backend

   Copyright (C) 1995, 1996 Free Software Foundation, Inc.

   Written by Miles Bader <miles@gnu.ai.mit.edu>

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   The GNU Hurd is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "store.h"

static error_t
zero_read (struct store *store,
	   off_t addr, size_t index, size_t amount, void **buf, size_t *len)
{
  if (*len < amount)
    {
      error_t err =
	vm_allocate (mach_task_self (), (vm_address_t *)buf, amount, 1);
      if (! err)
	*len = amount;
      return err;
    }
  else
    {
      bzero (*buf, amount);
      *len = amount;
      return 0;
    }
}

static error_t
zero_write (struct store *store,
	    off_t addr, size_t index, void *buf, size_t len, size_t *amount)
{
  return 0;
}

/* Modify SOURCE to reflect those runs in RUNS, and return it in STORE.  */
error_t
zero_remap (struct store *source,
	    const struct store_run *runs, size_t num_runs,
	    struct store **store)
{
  /* Because all blocks are the same, a zero store always contains just one
     run; here we simply count up the number of blocks specified by RUNS, and
     modify SOURCE's one run to reflect that.  */
  int i;
  off_t length = 0, old_length = source->runs[0].length;
  for (i = 0; i < num_runs; i++)
    if (runs[i].start < 0 || runs[i].start + runs[i].length >= old_length)
      return EINVAL;
    else
      length += runs[i].length;
  source->runs[0].length = length;
  *store = source;
  return 0;
}

error_t
zero_allocate_encoding (const struct store *store, struct store_enc *enc)
{
  enc->num_ints += 2;
  enc->num_offsets += 1;
  return 0;
}

error_t
zero_encode (const struct store *store, struct store_enc *enc)
{
  enc->ints[enc->cur_int++] = store->class->id;
  enc->ints[enc->cur_int++] = store->flags;
  enc->offsets[enc->cur_offset++] = store->size;
  return 0;
}

error_t
zero_decode (struct store_enc *enc, struct store_class *classes,
	     struct store **store)
{
  off_t size;
  int type, flags;

  if (enc->cur_int + 2 > enc->num_ints
      || enc->cur_offset + 1 > enc->num_offsets)
    return EINVAL;

  type = enc->ints[enc->cur_int++];
  flags = enc->ints[enc->cur_int++];
  size = enc->offsets[enc->cur_offset++];

  return store_zero_create (size, flags, store);
}

static struct store_class
zero_class =
{
  STORAGE_ZERO, "zero", zero_read, zero_write,
  zero_allocate_encoding, zero_encode, zero_decode,
  0, 0, 0, 0, zero_remap
};
_STORE_STD_CLASS (zero_class);

/* Return a new zero store SIZE bytes long in STORE.  */
error_t
store_zero_create (size_t size, int flags, struct store **store)
{
  struct store_run run = { 0, size };
  *store = _make_store (&zero_class, MACH_PORT_NULL, flags, 1, &run, 1, 0);
  return *store ? 0 : ENOMEM;
}