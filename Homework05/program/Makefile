CC      = gcc
CFLAGS  = -Wall -Wextra -pthread
TARGETS = thread_basic race_condition mutex_solution deadlock deadlock_fixed

all: $(TARGETS)

thread_basic: thread_basic.c
	$(CC) $(CFLAGS) -o $@ $^

race_condition: race_condition.c
	$(CC) $(CFLAGS) -o $@ $^

mutex_solution: mutex_solution.c
	$(CC) $(CFLAGS) -o $@ $^

deadlock: deadlock.c
	$(CC) $(CFLAGS) -o $@ $^

deadlock_fixed: deadlock_fixed.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	del /Q *.exe 2>nul || true

run_thread_basic: thread_basic
	./thread_basic

run_race_condition: race_condition
	./race_condition

run_mutex_solution: mutex_solution
	./mutex_solution

run_deadlock: deadlock
	@echo "按 Ctrl+C 中斷死結程式..."
	-./deadlock

run_deadlock_fixed: deadlock_fixed
	./deadlock_fixed

test: all
	@echo "=== 執行 thread_basic ==="
	thread_basic.exe
	@echo ""
	@echo "=== 執行 race_condition (3次，觀察變化) ==="
	race_condition.exe
	@echo ""
	race_condition.exe
	@echo ""
	race_condition.exe
	@echo ""
	@echo "=== 執行 mutex_solution ==="
	mutex_solution.exe
	@echo ""
	@echo "=== 執行 deadlock_fixed ==="
	deadlock_fixed.exe

.PHONY: all clean run_thread_basic run_race_condition run_mutex_solution run_deadlock run_deadlock_fixed test
