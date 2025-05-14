# Makefile for Dining Philosophers and Reader-Writer Problems

CC = gcc
CFLAGS = -Wall -pthread

# Executables
DINING_TARGET = dining_philosophers
READER_WRITER_TARGET = reader_writer

# Sources
DINING_SRC = dining_philosophers.c
READER_WRITER_SRC = reader_writer.c

RAILWAY_TARGET = railway_track
RAILWAY_SRC = railway_track.c

BANK_TARGET = bank_transaction
BANK_SRC = bank_transaction.c

PRODCONS_TARGET = producer_consumer
PRODCONS_SRC = producer_consumer.c

# Default rule: builds both
all: $(DINING_TARGET) $(READER_WRITER_TARGET) $(RAILWAY_TARGET) $(BANK_TARGET) $(PRODCONS_TARGET)

$(DINING_TARGET): $(DINING_SRC)
	$(CC) $(CFLAGS) -o $(DINING_TARGET) $(DINING_SRC)

$(READER_WRITER_TARGET): $(READER_WRITER_SRC)
	$(CC) $(CFLAGS) -o $(READER_WRITER_TARGET) $(READER_WRITER_SRC)

$(RAILWAY_TARGET): $(RAILWAY_SRC)
	$(CC) $(CFLAGS) -o $(RAILWAY_TARGET) $(RAILWAY_SRC)

$(BANK_TARGET): $(BANK_SRC)
	$(CC) $(CFLAGS) -o $(BANK_TARGET) $(BANK_SRC)

$(PRODCONS_TARGET): $(PRODCONS_SRC)
	$(CC) $(CFLAGS) -o $(PRODCONS_TARGET) $(PRODCONS_SRC)

run_bank:
	./$(BANK_TARGET)

run_dining: $(DINING_TARGET)
	./$(DINING_TARGET)

run_reader_writer: $(READER_WRITER_TARGET)
	./$(READER_WRITER_TARGET)

run_railway:
	./$(RAILWAY_TARGET)

run_prodcons:
	./$(PRODCONS_TARGET)

clean:
	rm -f $(DINING_TARGET) $(READER_WRITER_TARGET)