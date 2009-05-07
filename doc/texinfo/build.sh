#!/bin/bash

case "$1" in
	"pdf")
	pdftex min.texi
	pdftex min-reference.texi
	pdftex minqt-reference.texi
	;;
	"docbook")
	makeinfo --docbook min.texi
	makeinfo --docbook minqt-reference.texi
	makeinfo --docbook min-reference.texi
	;;
	"info")
	makeinfo min.texi
	makeinfo minqt-reference.texi
	makeinfo min-reference.texi
	;;
	"plaintext")
	makeinfo --plaintext --no-ifinfo --output=min.txt min.texi
	makeinfo --plaintext --no-ifinfo --output=min-reference.txt min-reference.texi
	makeinfo --plaintext --no-ifinfo --output=minqt-reference.txt min-reference.texi
	;;
	"html-s")
	makeinfo --html --css-include=min.css min.texi
	mkdir min/images/
	cp images/*.png min/images/
	makeinfo --html --css-include=min.css min-reference.texi
	makeinfo --html --css-include=min.css minqt-reference.texi
	mkdir minqt-reference/images/
	cp images/*.png minqt-reference/images/
	mkdir min-reference/images/
	cp images/*.png min-reference/images/
	;;
	#"html-c")
	#texi2html --node-files -split chapter -prefix min -top-file index.htm -toc-file contents.htm min.texi
	#ln -s images min/images
	#ln -s images min-reference/images
	#;;
	"html")
	makeinfo --no-split --css-include=min.css --html min.texi
	makeinfo --no-split --css-include=min.css --html minqt-reference.texi
	makeinfo --no-split --css-include=min.css --html min-reference.texi
	;;
	"all")
	for comm in  'pdf' 'html' 'html-s' 'docbook' 'info' 'plaintext' ; do 
		$0 $comm
	done
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
	echo "  all       - every format mentioned above"
	;;
esac


