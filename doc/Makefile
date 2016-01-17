# Makefile for the Maskeraid document in LaTex format.

version=0.9
Mug =  maskeraid_user_guide-$(version)
#default: $(Mug).pdf

INTERMEDIATE_FILES=$(Mug).toc \
                   $(Mug).out \
                   $(Mug).log \
                   $(Mug).aux


#.PHONY : $(Mug).pdf
$(Mug).pdf: 
	rm -f $(INTERMEDIATE_FILES) $(Mug).pdf
	pdflatex -interaction=batchmode -file-line-error $(Mug).tex
	pdflatex -interaction=batchmode -file-line-error $(Mug).tex
	pdflatex -interaction=batchmode -file-line-error $(Mug).tex

.PHONY: clean  cleanest
clean:
	rm -f $(INTERMEDIATE_FILES)
cleanest:
	rm -f $(INTERMEDIATE_FILES) $(Mug).pdf
