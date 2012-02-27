./waf clean
./waf linux
./waf
f="CLIENTBUILD"
rm -rf $f
mkdir $f
cp -R ext $f/
cp libc_lib.so $f/
cp run $f/
cp *.pyc $f/
rm $f/settings.pyc
cp -R media $f/
mkdir $f/lib
cp /usr/lib/i386-linux-gnu/libpng12.so.0 $f/lib/
cp README $f/
rm $f.tar.gz
tar cvzf $f.tar.gz $f
mv $f.tar.gz ~/Dropbox/Public
rm -rf ~/gs_build/$f
mv $f ~/gs_build/
