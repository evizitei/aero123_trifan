CC := g++ 
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TESTDIR := test

build:
	$(CC) -o $(BINDIR)/trifan $(SRCDIR)/TrifanHarness.cpp $(SRCDIR)/FlightController.cpp

clean:
	@echo " Cleaning..."; 
	rm $(BUILDDIR)/*
	rm $(BINDIR)/*

test:
	$(CC) -o $(TESTDIR)/bin/system_tests $(TESTDIR)/TrifanTest.cpp $(SRCDIR)/FlightController.cpp
	./test/bin/system_tests

.PHONY: clean build test