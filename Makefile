BASE = rantanplan
BISON = bison
CXX = g++
FLEX = flex

all: $(BASE)

%.cxx %.hxx: %.yy
	$(BISON) $(BISONFLAGS) -o $*.cxx $<

%.cxx: %.ll
	$(FLEX) $(FLEXFLAGS) -o$@ $<

%.o: %.cxx %.cpp
	$(CXX) $(CXXFLAGS) -c -o$@ $<

$(BASE): $(BASE).o driver.o parser.o scanner.o
	$(CXX) -o $@ $^

$(BASE).o: parser.hxx
parser.o: parser.hxx
scanner.o: parser.hxx

CLEANFILES = $(BASE) *.o parser.hh parser.cc location.hh scanner.cc
clean:
	rm -f $(CLEANFILES)
