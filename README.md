LinuxCkpttRecover
=================

this is a *simplified* implementation of checkpointing a running program
and running it again from the checkpointed point.

Its is a simplified implementation because it works only when the
address space randomization is disabled in kernel.

Files:
example.c : the input program file in which the main function is renamed as program_main
checkpoint.c : this is the file that takes care of checkpointing the current execution on pressing Ctrl+c
recover.c: this is the file that takes care of recovering the system state and start executing from the checkpointed state
common.c: takes care of command line parameters

Initially the address space randomization feature of the 2.6 kernel is put off with the following command
"sysctl -w kernel.randomize_va_space=0"
This makes sure that each execution of the program loads the stack area and heap area of the code in the same address
The above command should be executed with root privileges in both the host machine and remote machine

On pressing Ctrl+c

The checkpoint program saves the following states
setjmp call saves the register state. then manually the following are stored
1. the current stack till the setjmp call (dumps in the file ckpt.ckpt)
2. the current heap (dumps in the file ckpt.hckpt)
3. The jump buffer contents. (dumps in the file jmpbufinfo)

We dont have to save the text section as we run the same executable and they are mapped to the same region(as we have put off address space randomization)

the stack range and heap range are known by reading the /proc/pid/maps file

The recovery program 
reconstructs the heap
reconstructs the stack
reloads the jmpbuffer contents .
and does a long jmp

One subtle point with reconstructing the stack is that when we reconstruct the stack we may overwrite the address range , within which our current execution is happening. so to avoid that we use the widely known "recurse and restore" technique.
ie, the recover_stack function call is recursively called enough number of times so that the current execution happens far down below the stack and the reconstructing of the stack (whose address range will now fall safely in the top) happens now without problem

Compilation:gcc -w checkpoint.c common.c recover.c example.c
Execution:./a.out
Ctrl+C for checkpointing
Recover from checkpointed state: ./a.out -r

The controller:
-------------
 The controller gives a user friendly menu interface. The files are transferred through network via scp.
The user file is taken as input and the main function name is changed to program_main. The user file is linked with the checkpointing and recovery code to produce the executable.
compilation:gcc -w -o contrller controller.c 
Execution:./contrller

TODO:
====
. obvious TODO is to make it work with address space randomization ON
