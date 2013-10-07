#! /bin/sh

IOS_BASE_SDK="7.0"
IOS_DEPLOY_TARGET="7.0"
OUTPUT_DIR="."

mkdir -p $OUTPUT_DIR/include $OUTPUT_DIR/lib

setenv_all()
{
        # Add internal libs
        export CFLAGS="$CFLAGS -I$GLOBAL_OUTDIR/include -L$GLOBAL_OUTDIR/lib"

        export CPP="$TOOLCHAIN/usr/bin/cpp-4.2"
        export CXX="$TOOLCHAIN/usr/bin/g++-4.2"
        export CXXCPP="$TOOLCHAIN/usr/bin/cpp-4.2"
        export CC="$TOOLCHAIN/usr/bin/gcc-4.2"
        export LD=$TOOLCHAIN/usr/bin/ld
        export AR=$TOOLCHAIN/usr/bin/ar
        export AS=$TOOLCHAIN/usr/bin/as
        export NM=$TOOLCHAIN/usr/bin/nm
        export RANLIB=$TOOLCHAIN/usr/bin/ranlib
        export LDFLAGS="-L$SDKROOT/usr/lib/"

        export CPPFLAGS=$CFLAGS
        export CXXFLAGS=$CFLAGS
}

setenv_arm7()
{
        unset DEVROOT SDKROOT CFLAGS CC LD CPP CXX AR AS NM CXXCPP RANLIB LDFLAGS CPPFLAGS CXXFLAGS

        export TOOLCHAIN=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain
        export DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
        export SDKROOT=$DEVROOT/SDKs/iPhoneOS$IOS_BASE_SDK.sdk

        export CFLAGS="-arch armv7 -pipe -no-cpp-precomp -isysroot $SDKROOT -miphoneos-version-min=$IOS_DEPLOY_TGT -I$SDKROOT/usr/include/"

        setenv_all
}

setenv_i386()
{
        unset DEVROOT SDKROOT CFLAGS CC LD CPP CXX AR AS NM CXXCPP RANLIB LDFLAGS CPPFLAGS CXXFLAGS

        export DEVROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer
        export SDKROOT=$DEVROOT/SDKs/iPhoneSimulator$IOS_BASE_SDK.sdk

        export CFLAGS="-arch i386 -pipe -no-cpp-precomp -isysroot $SDKROOT -miphoneos-version-min=$IOS_DEPLOY_TGT"

        setenv_all
}

#
# libpng
#
cd libpng-1.6.6

# x86
setenv_i386
./configure '--enable-static=yes' \
            '--enable-shared=no'
make -j clean
make -j
cp .libs/libpng16.a ../lib/libpng-i386.a
