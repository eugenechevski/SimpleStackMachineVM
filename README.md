# Simple Stack Machine (SSM) Virtual Machine

This project implements a Simple Stack Machine (SSM) Virtual Machine based on Dr. Montagne's Stack VM and the MIPS processor.

## Requirements

- ANSI standard C
- gcc compiler
- Eustis environment (http://newton.i2lab.ucf.edu/wiki/Help:Eustis)

## Building

Use the provided Makefile:

```
    make vm
```

## Running Tests

Run all tests:

```
make check-outputs
```

Run specific tests:

```
make check-lst-outputs
make check-vm-outputs
```

## Usage

Print loaded program:

```
./vm -p <bof_file>
```

Execute program:

```
./vm <bof_file>
```

## Implementation Notes

- Implement VM memory and registers
- Implement instruction fetch-decode-execute cycle
- Implement all SSM instructions
- Implement loading of binary object files
- Implement tracing output
- Implement -p flag functionality
- Handle errors appropriately

## Submission

Create a submission zip file:

```
make submission.zip
```
