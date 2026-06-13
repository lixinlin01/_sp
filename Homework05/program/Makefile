CC      = gcc
CFLAGS  = -Wall -Wextra -pthread
TARGETS = bank producer_consumer dining_philosophers

all: $(TARGETS)

bank: bank.c
	$(CC) $(CFLAGS) -o $@ $^

producer_consumer: producer_consumer.c
	$(CC) $(CFLAGS) -o $@ $^

dining_philosophers: dining_philosophers.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	del /Q *.exe 2>nul || true

test: all
	@echo "=== Bank ==="
	bank.exe
	@echo ""
	@echo "=== Producer-Consumer ==="
	producer_consumer.exe
	@echo ""
	@echo "=== Dining Philosophers ==="
	dining_philosophers.exe

.PHONY: all clean test
