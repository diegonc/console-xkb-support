/* Implementation of memory_object_init for pager library
   Copyright (C) 1994, 1995, 1996 Free Software Foundation

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include "priv.h"
#include "memory_object_S.h"
#include <stdio.h>

/* Implement the object initialiation call as described in
   <mach/memory_object.defs>.  */
kern_return_t
_pager_seqnos_memory_object_init (mach_port_t object, 
				  mach_port_seqno_t seqno,
				  mach_port_t control,
				  mach_port_t name,
				  vm_size_t pagesize)
{
  struct pager *p;

  p = ports_lookup_port (0, object, _pager_class);
  if (!p)
    return EOPNOTSUPP;

  mutex_lock (&p->interlock);
  _pager_wait_for_seqno (p, seqno);

  if (pagesize != __vm_page_size)
    {
      printf ("incg init: bad page size");
      goto out;
    }

  if (p->pager_state != NOTINIT)
    {
#ifdef KERNEL_INIT_RACE
      struct pending_init *i = malloc (sizeof (struct pending_init));
      printf ("pager out-of-sequence init\n");
      i->control = control;
      i->name = name;

      i->next = 0;
      if (p->init_tail)
	p->init_tail->next = i;
      else
	p->init_head = i;
      p->init_tail = i;
#else
      printf ("pager dup init\n");
#endif      
      goto out;
    }

  p->memobjcntl = control;
  p->memobjname = name;

  memory_object_ready (control, p->may_cache, p->copy_strategy);

  p->pager_state = NORMAL;

 out:
  _pager_release_seqno (p, seqno);
  mutex_unlock (&p->interlock);
  ports_port_deref (p);

  return 0;
}
