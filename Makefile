CC := g++ 
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TESTDIR := test
BINNAME := trifan

build:
	$(CC) --std=c++11 -o $(BINDIR)/$(BINNAME) $(SRCDIR)/*.cpp

clean:
	@echo " Cleaning..."; 
	rm $(BUILDDIR)/*
	rm $(BINDIR)/*

run: build
	$(BINDIR)/$(BINNAME)

test:
	$(CC) --std=c++11 -o $(TESTDIR)/bin/system_tests $(TESTDIR)/TrifanTest.cpp $(SRCDIR)/FlightController.cpp $(SRCDIR)/FlightLog.cpp
	./test/bin/system_tests

.PHONY: clean build test