#!/bin/sh

if [ "$1" = "rpm" ]; then
    # A very simplistic RPM build scenario
    if [ -e rtl_sdr_device.spec ]; then
        mydir=`dirname $0`
        tmpdir=`mktemp -d`
        cp -r ${mydir} ${tmpdir}/rtl_sdr_device-1.0.0
        tar czf ${tmpdir}/rtl_sdr_device-1.0.0.tar.gz --exclude=".svn" -C ${tmpdir} rtl_sdr_device-1.0.0
        rpmbuild -ta ${tmpdir}/rtl_sdr_device-1.0.0.tar.gz
        rm -rf $tmpdir
    else
        echo "Missing RPM spec file in" `pwd`
        exit 1
    fi
else
    for impl in cpp ; do
        cd $impl
        if [ -e build.sh ]; then
            ./build.sh $*
        elif [ -e reconf ]; then
            ./reconf && ./configure && make
        else
            echo "No build.sh found for $impl"
        fi
        cd -
    done
fi
