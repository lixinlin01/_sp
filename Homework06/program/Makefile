CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGETS = 01_fork_basic 02_fork_exec 03_file_io 04_dup_redirection \
          05_pipe_communication 06_redirect_stdout_to_file 07_redirect_stdin_from_file

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
