# Biến cấu hình
CC = gcc
# -c: chỉ biên dịch, không liên kết. -Wall: bật tất cả cảnh báo. -I.: tìm header trong thư mục hiện tại.
CFLAGS = -c -Wall -I.
# Thư viện hệ thống
LIBS = -lm 
# Thư viện CMocka
CMOCKA_LIBS = -lcmocka

# Danh sách các file .o cần thiết cho Scanner functions (Dependencies)
DEP_OBJS = reader.o charcode.o token.o error.o

# Danh sách file .o cho Chương trình chính 'scanner'
MAIN_PROGRAM_OBJS = main.o scanner.o ${DEP_OBJS}

# Danh sách file .o cho Chương trình Test 'unit_test_scanner'
# CHÚ Ý: KHÔNG bao gồm scanner.o (vì unit_test_scanner.c đã include scanner.c) 
# và KHÔNG bao gồm main.o (vì unit_test_scanner.c có main của riêng nó).
TEST_RUNNER_OBJS = unit_test_scanner.o ${DEP_OBJS}


# =================================================================
# 1. Mục tiêu chính (Default target)
# =================================================================

.PHONY: all
all: scanner

# Tạo executable 'scanner' từ các file object
scanner: ${MAIN_PROGRAM_OBJS}
	${CC} ${MAIN_PROGRAM_OBJS} -o scanner ${LIBS}

# =================================================================
# 2. Mục tiêu Unit Test
# =================================================================

# Tạo executable 'unit_test_scanner'
unit_test_scanner: ${TEST_RUNNER_OBJS}
	${CC} ${TEST_RUNNER_OBJS} -o unit_test_scanner ${LIBS} ${CMOCKA_LIBS}

.PHONY: test
test: unit_test_scanner
	@echo "========================================"
	@echo "   Chạy Unit Test (unit_test_scanner)"
	@echo "========================================"
	./unit_test_scanner

# =================================================================
# 3. Quy tắc biên dịch file .c sang .o
# =================================================================

# Quy tắc chung: biên dịch tất cả các file .c thành .o
%.o: %.c
	${CC} ${CFLAGS} $< -o $@

# =================================================================
# 4. Mục tiêu dọn dẹp (Clean)
# =================================================================

.PHONY: clean
clean:
	@echo "Đang dọn dẹp..."
	rm -f *.o *~ scanner unit_test_scanner