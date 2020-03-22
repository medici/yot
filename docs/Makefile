PANDOC = pandoc
IFORMAT = markdown
MATHJAX = "http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"
FLAGS = --standalone --toc --toc-depth=2 --mathjax=$(MATHJAX) --highlight-style pygments
STYLE = css/style.css
TEMPLATE_HTML = template.htm
SRC = introduction.md install.md statement.md index.md
PNG_IMAGES = $(patsubst %.pdf,%.png,$(wildcard img/*.pdf))

OBJ = $(SRC:.md=.html)
img/%.png: img/%.pdf
	convert -density 150 $< $@
	
all: $(OBJ)

index: index.html

%.html: %.md $(FILTER)
	$(PANDOC) -c $(STYLE)  --metadata pagetitle="figure" --template $(TEMPLATE_HTML) -s -f $(IFORMAT) -t html $(FLAGS) -o $@ $<


clean:
	-rm *.html 