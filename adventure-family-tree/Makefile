all: adventure-family-tree.pdf

adventure-family-tree.pdf: adventure-family-tree.dot
	m4 < $< | dot -Tpdf > $@

clean:
	rm -f adventure-family-tree.pdf
