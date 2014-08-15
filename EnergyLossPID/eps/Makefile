EPSFILES = $(wildcard *.eps) $(wildcard */*.eps)
PDFFILES = $(patsubst %.eps,%.pdf,$(EPSFILES))

all : $(PDFFILES)

%.pdf : %.eps
	epstopdf $<
