# To get the `dot` command, try `brew install graphviz`

all: adventure-family-tree.pdf

adventure-family-tree.pdf: adventure-family-tree.dot
	m4 < $< | dot -Tpdf > $@

litclub.pdf: litclub.dot
	m4 < $< | dot -Tpdf > $@

clean:
	rm -f adventure-family-tree.pdf
