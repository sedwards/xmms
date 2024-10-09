#!/bin/bash
#set -e  # Exit immediately if any command fails

#glibtoolize -i -W none --verbose
#aclocal --install -W none
#autoupdate
#autoconf -f -W error
#automake -f -W none --add-missing


glibtoolize
aclocal
autoconf
automake --add-missing
./configure




