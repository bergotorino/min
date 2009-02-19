#!/bin/bash

tex min.texi
tex min-reference.texi
texindex min.??
texindex min-reference.??
tex min.texi
tex min-reference.texi
texindex min.??
texindex min-reference.??
tex min.texi
tex min-reference.texi

pdftex min.texi
pdftex min-reference.texi

texi2html min.texi
texi2html min-reference.texi

makeinfo min.texi
makeinfo min-reference.texi


