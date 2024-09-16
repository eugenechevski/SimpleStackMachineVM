CC = gcc
CFLAGS = -Wall -ansi -pedantic
LDFLAGS =
VM_OBJECTS = vm.o instruction.o bof.o utilities.o

.PHONY: all clean check-outputs check-lst-outputs check-vm-outputs

all: vm

vm: $(VM_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(VM_OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $

clean:
	rm -f *.o vm tests/*.myo tests/*.myp submission.zip

check-outputs: check-lst-outputs check-vm-outputs

check-lst-outputs: vm
	@echo "Running -p flag tests..."
	@for test in tests/*.bof; do \
		./vm -p $$test > $${test%.bof}.myp; \
		if diff -q $${test%.bof}.myp $${test%.bof}.lst > /dev/null; then \
			echo "PASS: $$test"; \
		else \
			echo "FAIL: $$test"; \
		fi; \
	done

check-vm-outputs: vm
	@echo "Running execution tests..."
	@for test in tests/*.bof; do \
		./vm $$test > $${test%.bof}.myo; \
		if diff -q $${test%.bof}.myo $${test%.bof}.out > /dev/null; then \
			echo "PASS: $$test"; \
		else \
			echo "FAIL: $$test"; \
		fi; \
	done

submission.zip: vm $(VM_OBJECTS:.o=.c) $(VM_OBJECTS:.o=.h) Makefile README.md
	zip $@ $^ tests/*.bof tests/*.lst tests/*.out

# Individual file dependencies
vm.o: vm.c vm.h machine_types.h instruction.h bof.h
instruction.o: instruction.c instruction.h machine_types.h
bof.o: bof.c bof.h machine_types.h
utilities.o: utilities.c utilities.h