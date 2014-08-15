pdf : report.pdf
dvi : report.dvi

INTER = -interaction batchmode

%.pdf : %.tex $(wildcard ../eps/*.pdf)
	pdflatex $(INTER) $<
	pdflatex $(INTER) $<

%.dvi : %.tex $(wildcard ../eps/*.eps)
	latex $(INTER) $<
	latex $(INTER) $<
	killall --signal SIGUSR1 xdvi.bin &

