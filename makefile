# 默认参数
SRCDIR	=	src
INCDIR	=	include
SOURCE	=	$(wildcard $(SRCDIR)/*.c)
HANDLE	=	$(wildcard $(INCDIR)/*.h)
CFLAGS	=	-I./include
CFLAGS	+=	-lcurl -lz -larchive -lm

all:get update

get		:$(filter-out $(SRCDIR)/update.c,	$(SOURCE)) $(HANDLE)
	gcc -o $@ $^ $(CFLAGS)
update	:$(filter-out $(SRCDIR)/get.c,		$(SOURCE)) $(HANDLE)
	gcc -o $@ $^ $(CFLAGS)

rm:clean
clean:
	rm ./get
	rm ./update
