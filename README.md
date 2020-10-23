# Operating Systems



This repository contains the assignments for the Academic Courses "Operating Systems" taught in the Fall of 2018-2019 in Aristotle University of Thessaloniki - Electrical and Computer Engineering.

The purpose of this project is the better understanding of **Linux shell** via the implementation a command line interpreter or **shell interpreter** and a **Makefile** in **C programming language**. 



---

###  Functionality

The shell that was requested should be able to understand Linux commands with or without arguments. Thus, is reasonable to create a parsing process for user's input stream. The functionality of our shell exceeds Linux command Line, because of the operators "**&&**" and "**;**". Using "**&&**" operator between two commands, the second command will be executed only if the first one is successfully executed. On the other hand, using "**;**" operator between two commands, the second command will be executed independently of the execution of the first one.  

Moreover the shell is able to:

- Redirect input via '<' character.
- Redirect output via '>' character.
- Create pipelines via '|' character.



---

### Modes

The shell should be able to work in two modes, **Interactive** and **Batch**.

- **Interactive**: shell is executed with no arguments

```C
./myshell
```

- **Batch**: shell is executed with a file (of commands) as argument

```C
./myshell batchfile
```



---



### Execution

A **Makefile** is used to execute our code. To compile the source code we can use:

```C
make all
```

while to run it:

```c
./bin/myshell
```



If a **batchfile** of commands should be used, we write instead:

```
./bin/myshell batchfile
```