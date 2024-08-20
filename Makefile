src := vEB_public.cpp vEB_private.cpp
objs := vEB_public.o vEB_private.o

bin:
	g++ -O3 -Wall -Wextra -I . -c $(src)

clean:
	rm -f $(objs)

all: $(objs)
	ar rcs lib_vEB.a $(objs)

