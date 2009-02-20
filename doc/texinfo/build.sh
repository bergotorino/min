#!/bin/bash

case "$1" in
	"pdf")
	pdftex min.texi
	pdftex min-reference.texi
	;;
	"docbook")
	makeinfo --docbook min.texi
	makeinfo --docbook min-reference.texi
	;;
	"info")
	makeinfo min.texi
	makeinfo min-reference.texi
	;;
	"plaintext")
	makeinfo --plaintext --no-ifinfo min.texi > min.txt
	makeinfo --plaintext --no-ifinfo min-reference.texi >min-reference.txt
	;;
	"html-s")
	makeinfo --html min.texi
	cp -r images min/
	makeinfo --html min-reference.texi
	cp -r images min-reference/
	;;
	#"html-c")
	#texi2html --node-files -split chapter -prefix min -top-file index.htm -toc-file contents.htm min.texi
	#ln -s images min/images
	#ln -s images min-reference/images
	#;;
	"html")
	makeinfo --no-split --html min.texi
	makeinfo --no-split --html min-reference.texi
	;;
	*)
	echo "USAGE: $0 <format>"
	echo "  Available formats:"
	echo "  pdf       - PDF files"
	echo "  docbook   - docbook XML files"
	echo "  info      - .info files"
	echo "  plaintext - plain text .txt files"
	#echo "  html-c    - HTML multi-file documentation,"
	#echo "              splitted to chapters"
	echo "  html-s    - HTML multi-file documentation,"
	echo "              splitted to nodes"
	echo "  html      - HTML single-file documentation"
	;;
esac


