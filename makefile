# 默认参数
SRCDIR	=	src
INCDIR	=	include
SOURCE	=	$(wildcard $(SRCDIR)/*.c)
HANDLE	=	$(wildcard $(INCDIR)/*.h)
CFLAGS	=	-I./include
CFLAGS	+=	-lcurl -lz -larchive -lm

all : gglibc

gglibc : $(SOURCE) $(HANDLE)
	gcc -o $@ $^ $(CFLAGS)

rm:clean
clean:
	rm ./gglibc