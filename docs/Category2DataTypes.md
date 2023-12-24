# Category 2 types

# Context and introduction
In Java, all types are subdivided into 2 categories.
Type 1 types are types that use 32 bit to store their values.
Type 2 types data types, 
are in memory and according to the specification defined as occupying two entries, wherever data is stored.
E.g. at runtime, two variables are occupied in memory where the programmer without internal JVM knowledge would expect just 1.
Category 2 variables are always referenced by their first index and cannot be interpreted as just one of the two parts by the Java programmer in normal operation.

The only category 2 types are long and double. All other types are category 1.

## Memory layout
## Stack
When category 2 data is pushed on the internal stack in our implementation, 
we push the least significant bits on the top of the stack.
So it is pushed last on the stack and will be the first to be popped.
The most significant bits are pushed first and will be popped last.
## Linear memory
In memory whenever we must store them in a linear fashion, we have the MSB first, and the LSB last.
This is the most logical choice because the data is also always stored in the class file in big endian format.
This is also the simplest form to understand as a human, since the bits are all in order if you would stick the data in the same order together into one QWORD.