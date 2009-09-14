libtoolize --force --copy
aclocal
autoheader
touch stamp-h
autoconf
automake -a -c
