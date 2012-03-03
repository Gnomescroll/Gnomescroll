./waf clean
./waf linux
./waf
version=`../get_current_head.sh`
f="gnomescroll_linux32_"$version
rm -rf $f
mkdir $f
cp -R ext $f/
cp libc_lib.so $f/
cp run $f/
cp *.pyc $f/
rm $f/settings.pyc
#cp default_settings.py $f/settings.py
cp -R media $f/
mkdir $f/lib
cp /usr/lib/i386-linux-gnu/libpng12.so.0 $f/lib/
cp README_linux $f/
rm $f.tar.gz
tar cvzf $f.tar.gz $f
mv $f.tar.gz ~/Dropbox/Public
rm -rf ~/gs_build/$f
mv $f ~/gs_build/
