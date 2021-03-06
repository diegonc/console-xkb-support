/*
 * Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990
 * Open Software Foundation, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of ("OSF") or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY
 * SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */

#include <alloca.h>
#include <mach/machine/vm_types.h>
#include <elf.h>
#include "mach-exec.h"

int exec_load(exec_read_func_t *read, exec_read_exec_func_t *read_exec,
		  void *handle, exec_info_t *out_info)
{
	vm_size_t actual;
	union { Elf32_Ehdr h; Elf64_Ehdr h64; } x;
	int i;
	int result;

	/* Read the ELF header.  */
	if ((result = (*read)(handle, 0, &x, sizeof(x), &actual)) != 0)
		return result;
	if (actual < sizeof(x))
		return EX_NOT_EXECUTABLE;

	if ((x.h.e_ident[EI_MAG0] != ELFMAG0) ||
	    (x.h.e_ident[EI_MAG1] != ELFMAG1) ||
	    (x.h.e_ident[EI_MAG2] != ELFMAG2) ||
	    (x.h.e_ident[EI_MAG3] != ELFMAG3))
		return EX_NOT_EXECUTABLE;

	/* Make sure the file is of the right architecture.  */
#ifdef i386
# define MY_CLASS	ELFCLASS32
# define MY_DATA	ELFDATA2LSB
# define MY_MACHINE	EM_386
#elif defined __alpha__
# define MY_CLASS	ELFCLASS64
# define MY_DATA	ELFDATA2LSB
# define MY_MACHINE	EM_ALPHA
#else
#error Not ported to this architecture!
#endif

	if ((x.h.e_ident[EI_CLASS] != MY_CLASS) ||
	    (x.h.e_ident[EI_DATA] != MY_DATA))
		return EX_WRONG_ARCH;

	if (MY_CLASS == ELFCLASS64)
	  {
	    Elf64_Phdr *phdr, *ph;
	    vm_size_t phsize;

	    if (x.h64.e_machine != MY_MACHINE)
	      return EX_WRONG_ARCH;

	    /* XXX others */
	    out_info->entry = (vm_offset_t) x.h64.e_entry;
	    out_info->init_dp = 0; /* ? */

	    phsize = x.h64.e_phnum * x.h64.e_phentsize;
	    phdr = (Elf64_Phdr *)alloca(phsize);

	    result = (*read)(handle, x.h64.e_phoff, phdr, phsize, &actual);
	    if (result)
	      return result;
	    if (actual < phsize)
	      return EX_CORRUPT;

	    for (i = 0; i < x.h64.e_phnum; i++)
	      {
		ph = (Elf64_Phdr *)((vm_offset_t)phdr + i * x.h64.e_phentsize);
		if (ph->p_type == PT_LOAD)
		  {
		    exec_sectype_t type = EXEC_SECTYPE_ALLOC |
		      EXEC_SECTYPE_LOAD;
		    if (ph->p_flags & PF_R) type |= EXEC_SECTYPE_READ;
		    if (ph->p_flags & PF_W) type |= EXEC_SECTYPE_WRITE;
		    if (ph->p_flags & PF_X) type |= EXEC_SECTYPE_EXECUTE;
		    result = (*read_exec)(handle,
					  ph->p_offset, ph->p_filesz,
					  ph->p_vaddr, ph->p_memsz, type);
		  }
	      }
	  }
	else
	  {
	    Elf32_Phdr *phdr, *ph;
	    vm_size_t phsize;

	    if (x.h.e_machine != MY_MACHINE)
	      return EX_WRONG_ARCH;

	    /* XXX others */
	    out_info->entry = (vm_offset_t) x.h.e_entry;
	    out_info->init_dp = 0; /* ? */

	    phsize = x.h.e_phnum * x.h.e_phentsize;
	    phdr = (Elf32_Phdr *)alloca(phsize);

	    result = (*read)(handle, x.h.e_phoff, phdr, phsize, &actual);
	    if (result)
	      return result;
	    if (actual < phsize)
	      return EX_CORRUPT;

	    for (i = 0; i < x.h.e_phnum; i++)
	      {
		ph = (Elf32_Phdr *)((vm_offset_t)phdr + i * x.h.e_phentsize);
		if (ph->p_type == PT_LOAD)
		  {
		    exec_sectype_t type = EXEC_SECTYPE_ALLOC |
		      EXEC_SECTYPE_LOAD;
		    if (ph->p_flags & PF_R) type |= EXEC_SECTYPE_READ;
		    if (ph->p_flags & PF_W) type |= EXEC_SECTYPE_WRITE;
		    if (ph->p_flags & PF_X) type |= EXEC_SECTYPE_EXECUTE;
		    result = (*read_exec)(handle,
					  ph->p_offset, ph->p_filesz,
					  ph->p_vaddr, ph->p_memsz, type);
		  }
	      }
	  }

	return 0;
}
