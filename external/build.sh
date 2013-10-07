#! /bin/sh

mkdir -p lib

cd freetype-2.5.0.1

# x86
./configure CFLAGS="-arch i386"
make clean
make
cp objs/.libs/libfreetype.a ../lib/libfreetype-i386.a
exit

# x64
./configure CFLAGS="-arch x86_64"
make clean
make
cp objs/.libs/libfreetype.a ../lib/libfreetype-x86_64.a

# armv7
./configure '--without-bzip2' \
            '--prefix=/usr/local/iphone' \
            '--host=arm-apple-darwin' \
            '--enable-static=yes' \
            '--enable-shared=no' \
            'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' \
            'CFLAGS=-arch armv7 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden \
                    -miphoneos-version-min=7.0 \
                    -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/usr/include/libxml2/ \
                    -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/' \
            'AR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar' \
            'LDFLAGS=-arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/ -miphoneos-version-min=7.0'
make clean
make
cp objs/.libs/libfreetype.a ../lib/libfreetype-arm7.a

# armv7s
./configure '--without-bzip2' \
            '--prefix=/usr/local/iphone' \
            '--host=arm-apple-darwin' \
            '--enable-static=yes' \
            '--enable-shared=no' \
            'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' \
            'CFLAGS=-arch armv7s -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden \
                    -miphoneos-version-min=7.0 \
                    -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/usr/include/libxml2/ \
                    -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/' \
            'AR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar' \
            'LDFLAGS=-arch armv7s -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/ -miphoneos-version-min=7.0'
make clean
make
cp objs/.libs/libfreetype.a ../lib/libfreetype-arm7s.a

# iOS
./configure '--without-bzip2' \
            '--prefix=/usr/local/iphone' \
            '--host=arm-apple-darwin' \
            '--enable-static=yes' \
            '--enable-shared=no' \
            'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' \
            'CFLAGS=-arch arm64 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden \
                    -miphoneos-version-min=7.0 \
                    -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/usr/include/libxml2/ \
                    -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/' \
            'AR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar' \
            'LDFLAGS=-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/ -miphoneos-version-min=7.0'
make clean
make
cp objs/.libs/libfreetype.a ../lib/libfreetype-arm64.a
