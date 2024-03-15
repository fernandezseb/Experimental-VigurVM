# VigurVM

![Logo of the project, showing a simple drawing inspired by a famous windmill in Vigur, Iceland](Vigur_logo.svg)

A simple java virtual machine made for learning purposes.


## Goal and scope

### VigurVM

The goal of this project is to be a simple to understand C++ implementation of a Java Virtual Machine, in order to learn more about Java and C++. 
(Because I haven't worked professionally with C++ yet.)

The goal is **not** to be reliable, fast, or memory efficient, or even feature complete.
Even though it is not set in stone, I will be trying to implement as much features as possible of JDK 8, in the free time that I have.

### ClassDump

In the [/tools/ClassDump](/tools/ClassDump) subdirectory is a custom tool similar to javap that shares some code with VigurVM. (Although there is not much reuse of code at the moment between VigurVM and the ClassDump tool. This will also happen over time.)
The development of ClassDump is less important but will slowly continue alongside with VigurVM.
For any further information about this project, please refer to the README in the project's subdirectory [here](/tools/ClassDump).

## Current state

Some simple programs can work, but some basic things like printing text using the standard System.out PrintWriter doesn't work yet.
The current goal is to get all the instructions, and native code working to get the System.out PrintWriter Object created, as well as all the requirements for this Object. 
And get the output connected to the user's standard out.
Alongside that, there are still some major things that need to be done:

* Implement [class file verification](https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.10)
* Implement more [instructions](https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-6.html) (and improve the already created ones)
* Implement a proper garbage collector infrastructure and a simple garbage collector
* Support [custom ClassLoaders](https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-5.html#jvms-5.3.2) (Now only the boostrapping classloader works)
* Implement multithreading logic in VM
* Implement full [Format Checking](https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.8)