#2.2
c. `fork()` creates  a  new  process. The return value is the zero in the child and the process-id number of the child in the parent, or -1 upon error.
e. No, the `fork()` system call only creates the process. We still need to call the `execvp()` system call to launch the program.

