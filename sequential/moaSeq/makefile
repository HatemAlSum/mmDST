moaSeq : moaSq.o moamsa.o moa.o utils.o scores.o lq.o
	gcc -lpthread ../moaSq.c ../moamsa.c ../moa.c ../utils.c ../scores.c ../lq.c -o moaSeq

moaSq.o : ../moaSq.c ../moa.h ../utils.h ../moamsa.h
	cc -c ../moaSq.c

moamsa.o : ../moamsa.c ../moa.h ../scores.h ../utils.h ../moamsa.h
	cc -c ../moamsa.c

moa.o :  ../moa.c ../moa.h ../utils.h ../moamsa.h ../lq.h
	cc -c ../moa.c

utils.o : ../utils.c ../moa.h ../utils.h ../moamsa.h
	cc -c ../utils.c

scores.o : ../scores.c ../scores.h
	cc -c ../scores.c

lq.o : ../lq.c ../lq.h
	cc -c ../lq.c

clean : 
	rm moaSeq moaSeq.o  moamsa.o moa.o utils.o scores.o lq.o