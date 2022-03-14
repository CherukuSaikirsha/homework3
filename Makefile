# Make file for the source code

all: clean homework3
  
clean:: rm -fr homework3

homework3:
 gcc  -o homework3 homework3.c


