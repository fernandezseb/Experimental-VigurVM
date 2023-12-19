# ClassDump

A simple java classfile parser made for learning purposes.
The output is very similar to the one from the javap tool with all output enabled. (Private members, system info about the class, etc.)

There is a blogpost introducing this project on my website over [here](https://sebfernandez.be/programming/a-simple-class-file-parser/)

## Example

```bash
./ClassDump.exe test/Main.class
```

```
Classfile C:/ClasDump/Main.class
  Last modified Nov 03, 2023; size 687 bytes
  MD5 checksum 433e9b297a1f75d82521b577f9482758
  Compiled from "Main.java"
public class Main implements java.io.Serializable
  minor version: 0
  major version: 52
  flags: ACC_PUBLIC, ACC_SUPER
Constant pool:
   #1 = Methodref      #6.#26         // java/lang/Object."<init>":()V
   #2 = Fieldref       #27.#28        // java/lang/System.out:Ljava/io/PrintStream;
   #3 = String         #29            // Hello world!
   #4 = Methodref      #30.#31        // java/io/PrintStream.println:(Ljava/lang/String;)V
   #5 = Class          #32            // Main
   #6 = Class          #33            // java/lang/Object
   #7 = Class          #34            // java/io/Serializable
   #8 = Utf8           <init>
   #9 = Utf8           (Ljava/lang/String;)V
  #10 = Utf8           Code
  #11 = Utf8           LineNumberTable
  #12 = Utf8           LocalVariableTable
  #13 = Utf8           this
  #14 = Utf8           LMain;
  #15 = Utf8           str
  #16 = Utf8           Ljava/lang/String;
  #17 = Utf8           main
  #18 = Utf8           ([Ljava/lang/String;)V
  #19 = Utf8           args
  #20 = Utf8           [Ljava/lang/String;
  #21 = Utf8           j
  #22 = Utf8           I
  #23 = Utf8           StackMapTable
  #24 = Utf8           SourceFile
  #25 = Utf8           Main.java
  #26 = NameAndType    #8.#35         // "<init>":()V
  #27 = Class          #36            // java/lang/System
  #28 = NameAndType    #37.#38        // out:Ljava/io/PrintStream;
  #29 = Utf8           Hello world!
  #30 = Class          #39            // java/io/PrintStream
  #31 = NameAndType    #40.#9         // println:(Ljava/lang/String;)V
  #32 = Utf8           Main
  #33 = Utf8           java/lang/Object
  #34 = Utf8           java/io/Serializable
  #35 = Utf8           ()V
  #36 = Utf8           java/lang/System
  #37 = Utf8           out
  #38 = Utf8           Ljava/io/PrintStream;
  #39 = Utf8           java/io/PrintStream
  #40 = Utf8           println
{
  public Main(java.lang.String);
    descriptor: (Ljava/lang/String;)V
    flags: ACC_PUBLIC
    Code:
      stack=1, locals=2, args_size=2
         0: aload_0
         1: invokespecial #1                      // Method java/lang/Object."<init>":()V
         4: return
      LineNumberTable:
        line 4: 0
        line 5: 4
      LocalVariableTable:
        Start  Length  Slot  Name   Signature
            0       5     0  this   LMain;
            0       5     1   str   Ljava/lang/String;

  public static void main(java.lang.String[]);
    descriptor: ([Ljava/lang/String;)V
    flags: ACC_PUBLIC, ACC_STATIC
    Code:
      stack=2, locals=2, args_size=1
         0: bipush        9
         2: istore_1
         3: iload_1
        15: lookupswitch  { // 2
                       5: 43
                      10: 32
                 default: 46
            }
        32: getstatic     #2                      // Field java/lang/System.out:Ljava/io/PrintStream;
        35: ldc           #3                      // string Hello world!
        37: invokevirtual #4                      // Method java/io/PrintStream.println:(Ljava/lang/String;)V
        40: goto          #6                      // class java/lang/Object
        43: iinc          1 1
        46: return
      LineNumberTable:
        line 8: 0
        line 10: 3
        line 12: 32
        line 13: 40
        line 15: 43
        line 18: 46
      LocalVariableTable:
        Start  Length  Slot  Name   Signature
            0      47     0  args   [Ljava/lang/String;
            3      44     1     j   I

}
SourceFile: "Main.java"
```


## Goal and scope

The goal of this project is to be a simple to understand C++ implementation of a Java class file parser, in order to learn more about Java class files and C++. 
(Because I haven't worked professionally with C++ yet.)
In the future I am going to use this one and create a simple to understand basic implementation of a JVM using this parser, also with the aim of being educational.

The goal is **not** to be reliable, fast, or memory efficient, or even feature complete.
Even though it is not set in stone, I will be trying to implement as much features as possible of JDK 8, in the free time that I have.

## Still to do
* The last opcodes that are not implemented yet: lookuptable, wide
* Implement full unicode support
