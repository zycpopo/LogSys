# 定义编译器和编译选项
CC = g++
CFLAGS = -Wall -g

# 最终生成的程序名
TARGET = test

# 所有需要编译的源文件
SRC = test.cc

# 自动生成对应的 .o 文件
OBJ = $(SRC:.cc=.o)

# 编译所有 .cc 文件为 .o 文件
%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@ 

# 生成最终程序 + 自动删除 .o 文件
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -pthread
	rm -f $(OBJ)

# 清理编译产物
clean:
	rm -f $(OBJ) $(TARGET)