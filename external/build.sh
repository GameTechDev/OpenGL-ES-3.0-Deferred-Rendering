#! /bin/sh

cd freetype-2.5.0.1
./configure '--without-bzip2' \
            '--prefix=/usr/local/iphone' \
            '--host=arm-apple-darwin' \
            '--enable-static=yes' \
            '--enable-shared=no' \
            'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' \
            'CFLAGS=-arch armv7 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden \
                    -miphoneos-version-min=5.1 \
                    -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/usr/include/libxml2/ \
                    -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/' \
            'AR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar' \
            'LDFLAGS=-arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk/ -miphoneos-version-min=5.1'
