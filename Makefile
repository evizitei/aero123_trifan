# TODO: Add more tasks as project fleshes out
CC := g++ 
SRCDIR := src
BUILDDIR := build

clean:
  @echo " Cleaning..."; 
  rm -r $(BUILDDIR)

.PHONY: clean