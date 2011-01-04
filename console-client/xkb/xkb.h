/*  Keyboard plugin for the Hurd console using XKB keymaps.

    Copyright (C) 2002,03  Marco Gerards
   
    Written by Marco Gerards <marco@student.han.nl>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.  */

#include <errno.h>
#include <argp.h>
//#include "kbd_driver.h"
#include <X11/extensions/XKBcommon.h>

typedef int keycode_t;
typedef unsigned int scancode_t;
typedef int symbol;
typedef int group_t;
typedef unsigned int boolctrls;
//typedef int error_t;

#define	KEYCONSUMED	1
#define	KEYNOTCONSUMED	0

#define RedirectIntoRange  1
#define ClampIntoRange     2
#define WrapIntoRange      0

typedef unsigned long KeySym;

/* Real modifiers.  */
#define RMOD_CTRL	1 << 2
#define RMOD_MOD1	1 << 3

/* The complete set of modifiers.  */
typedef struct modmap
{
  /* Real modifiers.  */
  int rmods;
  /* Virtual modifiers.  */
  int vmods;
} modmap_t;

/* Modifier counter.  */
typedef struct modcount
{
  int rmods[8];
  int vmods[16];
} modcount_t;

/* All Actions as described in the protocol specification.  */
typedef enum actiontype
  {
    SA_NoAction,
    SA_SetMods,
    SA_LatchMods,
    SA_LockMods,
    SA_SetGroup,
    SA_LatchGroup,
    SA_LockGroup,
    SA_MovePtr,
    SA_PtrBtn,
    SA_LockPtrBtn,
    SA_SetPtrDflt,
    SA_ISOLock,
    SA_TerminateServer,
    SA_SwitchScreen,
    SA_SetControls,
    SA_LockControls,
    SA_ActionMessage,
    SA_RedirectKey,
    SA_DeviceBtn,
    SA_LockDeviceBtn,
    SA_DeviceValuator,
    SA_ConsScroll
  } actiontype_t;

#define	useModMap	4
#define	clearLocks	1
#define	latchToLock	2   
#define	noLock		1
#define	noUnlock	2      
#define	groupAbsolute	4
#define	NoAcceleration	1
#define	MoveAbsoluteX	2
#define	MoveAbsoluteY	4

extern struct xkb_desc *xkb_desc;

/* The current state of every key.  */
typedef struct keystate
{
  /* Key is pressed.  */
  unsigned short keypressed:1;
  unsigned short prevstate:1;
  /* The key was disabled for bouncekeys.  */
  unsigned short disabled:1;
  /* Information about locked modifiers at the time of the keypress,
     this information is required for unlocking when the key is released.  */
  modmap_t lmods;
  /* The modifiers and group that were active at keypress, make them
     active again on keyrelease so the action will be undone.  */
  modmap_t prevmods;
  boolctrls bool;
  group_t prevgroup;
  group_t oldgroup;
} keystate_t;

extern struct keystate keystate[255];

typedef struct keypress
{
  keycode_t keycode;
  keycode_t prevkc;
  //  struct keystate *state;
  unsigned short repeat:1;	/* It this a real keypress?.  */
  unsigned short redir:1;	/* This is not a real keypress.  */
  unsigned short rel;		/* Key release.  */
} keypress_t;

/* Flags for indicators.  */
#define	IM_NoExplicit	0x80
#define	IM_NoAutomatic	0x40
#define	IM_LEDDrivesKB	0x20

#define	IM_UseCompat	0x10
#define	IM_UseEffective	0x08
#define	IM_UseLocked	0x04
#define	IM_UseLatched	0x02
#define	IM_UseBase	0x01

unsigned int KeySymToUcs4(int keysym);
symbol compose_symbols (symbol symbol);
error_t read_composefile (char *);

error_t xkb_input_key (int key);

error_t xkb_init_repeat (int delay, int repeat);

void xkb_input (keypress_t key);

int debug_printf (const char *f, ...);

error_t xkb_load_layout (char *xkbdir, char *xkbkeymapfile, char *xkbkeymap);
