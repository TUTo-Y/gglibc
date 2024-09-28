# 默认参数
SRCDIR	=	src
INCDIR	=	include
OBJDIR  =   obj
SOURCE	=	$(wildcard $(SRCDIR)/*.c)
HANDLE	=	$(wildcard $(INCDIR)/*.h)
OBJECTS =   $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCE))
CFLAGS	=	-I./include
CFLAGS	+=	-lcurl -lz -larchive -lm

# 链接目标文件生成可执行文件
all : gglibc

gglibc : $(OBJECTS)
	gcc -o $@ $^ $(CFLAGS)

# 生成目标文件目录
$(OBJDIR):
	mkdir -p $(OBJDIR)

# 目标文件规则
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HANDLE) | $(OBJDIR)
	gcc -c $< -o $@ $(CFLAGS)

# 清理目标文件和可执行文件
clean:
	rm -rf $(OBJDIR) gglibc