#!/bin/bash

# clear down old html files
rm -f ~/repos/cite/live/pages/*.html
rm -f ~/repos/cite/src/html/*.html

# convert markdown to html
for i in ~/repos/cite/src/md/*md; do
	name=$(echo $i | cut -d. -f1) # don't put periods in file names
	markdown -o $name.html $i
done

mv ~/repos/cite/src/md/*.html ~/repos/cite/src/html

# build the website
cite

