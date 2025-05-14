CC = gcc
CFLAGS = -Wall -Wextra -O2

all: srtf sjf round_robin virtual_round_robin

srtf: srtf.c
	$(CC) $(CFLAGS) -o srtf srtf.c

sjf: sjf.c
	$(CC) $(CFLAGS) -o sjf sjf.c

round_robin: round_robin.c
	$(CC) $(CFLAGS) -o round_robin round_robin.c

virtual_round_robin: virtual_round_robin.c
	$(CC) $(CFLAGS) -o virtual_round_robin virtual_round_robin.c

run_srtf: srtf
	./srtf > srtf_output.txt 2>&1

run_sjf: sjf
	./sjf > sjf_output.txt 2>&1

run_rr: round_robin
	./round_robin > round_robin_output.txt 2>&1

run_vrr: virtual_round_robin
	./virtual_round_robin > virtual_round_robin_output.txt 2>&1

clean:
	rm -f srtf sjf round_robin virtual_round_robin srtf_output.txt sjf_output.txt round_robin_output.txt virtual_round_robin_output.txt