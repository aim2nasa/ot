all:
	@cd hello;make
	@cd keygen;make
	@cd persistentObj;make
	@cd storage;make

clean:
	@cd hello;make clean
	@cd keygen;make clean
	@cd persistentObj;make clean
	@cd storage;make clean
