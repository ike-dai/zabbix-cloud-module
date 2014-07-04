cloud_module: cloud_module.c
	gcc -shared -o cloud_module.so cloud_module.c ../../libs/zbxmemory/memalloc.o -I../../../include -ldeltacloud -fPIC
