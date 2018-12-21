objects = parse.o main.o
target = proc

proc : $(objects)
	cc -o $(target) $(objects)
	rm $(objects)

%.o : sch.h

.PYONY :clean
clean :
	-rm $(objects) $(target)