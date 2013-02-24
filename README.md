LinuxCkpttRecover
=================

this is a *simplified* implementation of checkpointing a running program
and running it again from the checkpointed point.

Its is a simplified implementation because it works only when the
address space randomization is disabled in kernel.

"sysctl -w kernel.randomize_va_space=0"

When you run the controller executable,
it asks for your program.c that you want to run-checkpoint-and-continue
it produces a.out after linking with the checkpoint.c and recover.c
you can check as follows
1. run the ./a.out,
2. press ctrl-c
3. continue from the checkpoint by typing ./a.out -r
(see the controller.c for more details)

TODO:
====
. obvious TODO is to make it work without turning off address space randomisation.
