BASE = rantanplan
BISON = bison
CXX = g++
FLEX = flex

all: $(BASE)

parser.cxx parser.hxx: src/parser/parser.yy
	$(BISON) $(BISONFLAGS) -o $*.cxx $<

scanner.cxx: src/parser/scanner.ll
	$(FLEX) $(FLEXFLAGS) -o$@ $<

%.o: %.cxx %.cpp
	$(CXX) $(CXXFLAGS) -c -o$@ $<

$(BASE): $(BASE).o driver.o parser.o scanner.o
	$(CXX) -o $@ $^

$(BASE).o: parser.hxx
parser.o: parser.hxx
scanner.o: parser.hxx

CLEANFILES = $(BASE) *.o parser.hh parser.cxx location.hxx scanner.cxx
clean:
	rm -f $(CLEANFILES)
