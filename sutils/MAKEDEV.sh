#!/bin/sh
#
# Make standard devices
#

PATH=/bin

function st {
  NODE="$1"
  OWNER="$2"
  PERM="$3"
  shift 3
  settrans -cg "$NODE"
  chown "$OWNER" "$NODE"
  chmod "$PERM" "$NODE"
  settrans "$NODE" "$@"
}

_CWD=${_CWD:-`pwd`}
export _CWD

for I; do
  case "$I" in
    std)
      $0 console tty null zero fd time
      ;;
    console|tty[0-9][0-9a-f]|tty[0-9a-f]|com[0-9])
      st $I root 600 /hurd/term $_CWD/$I device $I;;
    null)
      st $I root 666 /hurd/null;;
    zero)
      st $I root 666 /hurd/null -z;;
    tty)
      st $I root 666 /hurd/magic tty;;
    fd)
      st $I root 666 /hurd/magic fd
      ln -f -s fd/0 stdin
      ln -f -s fd/1 stdout
      ln -f -s fd/2 stderr
      ;;
    time)
      st $I root 666 /hurd/devport time ;;

    # ptys
    [pt]ty[pqPQ]?)
      # Make one pty, both the master and slave halves
      ID="`expr substr $I 4 99`"
      st pty$ID root 640 /hurd/term $_CWD/pty$ID pty-master $_CWD/tty$ID
      st tty$ID root 640 /hurd/term $_CWD/tty$ID pty-slave $_CWD/pty$ID
      ;;
    [pt]ty[pqPQ])
      # Make a bunch of ptys
      $0 ${I}0 ${I}1 ${I}2 ${I}3 ${I}4 ${I}5 ${I}6 ${I}7
      $0 ${I}8 ${I}9 ${I}a ${I}b ${I}c ${I}d ${I}e ${I}f
      ;;

    fd*|mt*)
      st $I root 640 /hurd/storeio -d $I
      ;;

    [hrs]d*)
      case "$I" in
      [a-z][a-z][0-9][a-z] | [a-z][a-z][0-9]s[1-9] | [a-z][a-z][0-9]s[1-9][a-z] | [a-z][a-z][0-9])
        st $I root 640 /hurd/storeio -d $I
	;;
      *)
	echo 1>&2 $0: $I: Illegal device name: must supply a device number
	exit 1
	;;
      esac
      ;;

    *)
      echo >&2 $0: $I: Unknown device
      exit 1
      ;;
  esac
done