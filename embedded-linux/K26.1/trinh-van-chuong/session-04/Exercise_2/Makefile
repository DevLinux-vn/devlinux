CC = gcc
CFLAGS = -Wall -Wextra

# Target mặc định khi gõ lệnh 'make' hoặc 'make all'
all: manager searcher

# Biên dịch chương trình chính của cha
manager: manager.c
	$(CC) $(CFLAGS) -o manager manager.c

# Biên dịch chương trình của con
searcher: searcher.c
	$(CC) $(CFLAGS) -o searcher searcher.c

# Lệnh dọn dẹp file rác
clean:
	rm -f manager searcher