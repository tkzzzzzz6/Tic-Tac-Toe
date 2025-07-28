# Tic-Tac-Toe Game Makefile

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -leasyx -static-libgcc -static-libstdc++

# 目录设置
SRCDIR = EasyX_Demo
BUILDDIR = exe
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
TARGETS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.exe)

# 默认目标
all: $(BUILDDIR) $(TARGETS)

# 创建构建目录
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# 编译规则
$(BUILDDIR)/%.exe: $(SRCDIR)/%.cpp
	@echo "Building $<..."
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✓ $@ built successfully!"

# 清理目标
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILDDIR)
	@echo "✓ Clean completed!"

# 运行游戏
run: $(BUILDDIR)/Tic-Tac-Toe.exe
	@echo "Running Tic-Tac-Toe game..."
	./$(BUILDDIR)/Tic-Tac-Toe.exe

# 运行演示
demo: $(BUILDDIR)/demo1.exe
	@echo "Running mouse interaction demo..."
	./$(BUILDDIR)/demo1.exe

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  all     - Build all executables (default)"
	@echo "  clean   - Remove all build files"
	@echo "  run     - Build and run the Tic-Tac-Toe game"
	@echo "  demo    - Build and run the mouse interaction demo"
	@echo "  help    - Show this help message"

# 检查依赖
check-deps:
	@echo "Checking dependencies..."
	@which $(CXX) > /dev/null || (echo "Error: $(CXX) not found!" && exit 1)
	@echo "✓ $(CXX) found"
	@echo "Note: Make sure EasyX library is installed"

# 安装目标（如果需要）
install: all
	@echo "Installation not implemented yet"
	@echo "Please copy executables from $(BUILDDIR) to your desired location"

.PHONY: all clean run demo help check-deps install 