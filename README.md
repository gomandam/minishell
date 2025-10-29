### **minishell**
Replicates the bash/shell functions, understanding UNIX/Linux command line, and hands-on experience with system calls.
This lightweight shell features command execution, piping, redirection, and environment variable management.  
> Authored by [angelurano](https://github.com/angelurano/miniyeska) and myself [g01100001](https://github.com/gomandam)

---

<div align=center>
<h3 align="center">Allowed Functions</h3>
<h3>Used previously</h3>

| Function				| Manual Page		| From lib			| Description
| :--					| :--				| :--				| :--
| **printf**			| `man 3 printf`	| `<stdio.h>`		| write output to stdout
| **malloc**			| `man malloc`		| `<stdlib.h>`		| allocate dynamic memory
| **free**				| `man 3 free`		| `<stdlib.h>`		| free dynamic memory
| **read**				| `man 2 read`		| `<unistd.h>`		| read from a file descriptor
| **write**				| `man 2 write`		| `<unistd.h>`		| write to a file descriptor
| **open**				| `man 2 open`		| `<fcntl.h>`		| open and possibly create a file
| **close**				| `man 2 open`		| `<unistd.h>`		| close a file descriptor
| **fork**				| `man fork`		| `<unistd.h>`		| create a child process
| **wait**				| `man wait`		| `<sys/wait.h>`	| wait for process to change state
| **waitpid**			| `man waitpid`		| `<sys/wait.h>`	| wait for process to change state
| **exit**				| `man exit`		| `<stdlib.h>`		| cause normal process termination
| **execve**			| `man execve`		| `<unistd.h>`		| execute program
| **dup**				| `man dup`			| `<unistd.h>`		| duplicate a file descriptor
| **dup2**				| `man dup2`		| `<unistd.h>`		| duplicate a file descriptor
| **pipe**				| `man pipe`		| `<unistd.h>`		| create pipe
| **strerror**			| `man strerror`	| `<string.h>`		| return string describing error number
| **errno**				| `man errno`		| `<errno.h>`		| number of last error
| **access**			| `man access`		| `<unistd.h>`		| checks the access permissions
| **perror**			| `man perror`		| `<stdio.h>`		| look up the error message string corresponding to an error

---
<div align=center>
<h3>New Functions</h3>

| Function				| Manual Page		| From lib			| Description
| :--					| :--				| :--				| :--
| **getenv**			|					|					| getting the value of variables from the env list
| **wait3**				| `man wait3`		| `<sys/wait.h>`	| (obsolete) wait for process to change state, BSD style
| **wait4**				| `man wait4`		| `<sys/wait.h>`	| (obsolete) wait for process to change state, BSD style
| **signal**			| `man signal`		| `<signal.h>`		| ANSI C signal handling
| **kill**				| `man 2 kill`		| `<signal.h>`		| send signal to a process
| **getcwd**			| `man getcwd`		| `<unistd.h>`		| get current working directory
| **chdir**				| `man chdir`		| `<unistd.h>`		| change working directory
| **stat**				| `man 2 stat`		| `<sys/stat.h>`	| get file status by pathname
| **lstat**				| `man lstat`		| `<sys/stat.h>`	| get file status by pathname (for symlinks)
| **fstat**				| `man fstat`		| `<sys/stat.h>`	| get file status by fd
| **opendir**			| `man opendir`		| `<dirent.h>`		| open a directory
| **readdir**			| `man readdir`		| `<dirent.h>`		| read a directory
| **closedir**			| `man closedir`	| `<dirent.h>`		| close a directory
| **isatty**			| `man isatty`		| `<unistd.h>`		| determines if the file descriptor fd refers to a valid terminal
| **ttyname**			| `man ttyname`		| `<unistd.h>`		| gets the related device name of a file descriptor for which isatty() is true
| **ttyslot**			| `man ttyslot`		| `<unistd.h>`		| returns the name stored in a static buffer which will be overwritten on subsequent calls
| **ioctl**				| `man ioctl`		| `<sys/ioctl.h>`	| manipulates the underlying device parameters of special files
| **unlink**			| `man unlink`		| `<unistd.h>`		| deletes a name from the filesystem
| **sigaction**			| `man sigaction`	| `<signal.h>`		| assigns an action for a signal specified by sig
| **sigemptyset**		| `man sigemptyset`	| `<signal.h>`		| initializes a signal set to be empty
| **sigaddset**			| `man sigaddset`	| `<signal.h>`		| adds the specified signal signo to the signal set
| **tcsetattr**			| `man tcsetattr`	| `<termios.h>`		| setting the termios structure
| **tcgetattr**			| `man tcgetattr`	| `<termios.h>`		| getting the termios structure
| **tgetent**			| `man tgetent`		| `<curses.h>`   `<term.h>`  | loads the entry for name
| **tgetflag**			| `man tgetflag`	| `<curses.h>`   `<term.h>`  | gets the boolean entry for id, or zero if it is not available
| **tgetnum**			| `man tgetnum`		| `<curses.h>`   `<term.h>`  | gets the numeric entry for id, or -1 if it is not available
| **tgetstr**			| `man tgetstr`		| `<curses.h>`   `<term.h>`  | returns the string entry for id, or zero if it is not available
| **tgoto**				| `man tgoto`		| `<curses.h>`   `<term.h>`  | instantiates the parameters into the given capability
| **tputs**				| `man tputs`		| `<curses.h>`   `<term.h>`  | applies  padding  information  to the string str and outputs it
| **readline**			| 					| `<stdio.h>`   `<readline/readline.h>`   `<readline/history.h>`  | will read a line from the terminal and return it, using prompt as a prompt
| **rl_clear_history**	| 					| `<stdio.h>`   `<readline/readline.h>`   `<readline/history.h>`  | Clear the history list by deleting all of the entries
| **rl_on_new_line**	| 					| `<stdio.h>`   `<readline/readline.h>`   `<readline/history.h>`  | Tell the update functions that we have moved onto a new (empty) line, usually after outputting a newline
| **rl_replace_line**	| 					| `<stdio.h>`   `<readline/readline.h>`   `<readline/history.h>`  | Replace the contents of rl_line_buffer with text
| **rl_redisplay**		| 					| `<stdio.h>`   `<readline/readline.h>`   `<readline/history.h>`  | Change what’s displayed on the screen to reflect the current contents of rl_line_buffer
| **add_history**		|					| `<stdio.h>`   `<readline/readline.h>`   `<readline/history.h>`  | Saving the line into the history

---

> Table Reference: [zstenger93](https://github.com/zstenger93/minishell)
