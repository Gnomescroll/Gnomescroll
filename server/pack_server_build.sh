./waf clean
./waf linux
./waf
version=`../get_current_head.sh`
f="gnomescroll_linux32_server_"$version
rm -rf $f
mkdir $f/
cp -R ext $f/
cp libc_lib.so $f/
cp *.pyc $f/
rm settings.pyx $f/
cp run $f/
mkdir $f/media
cp -R ../netclient/media/vox $f/media/
cp -R ../netclient/media/voxel $f/media/
cp -R ../netclient/media/maps $f/media/
rm $f.tar.gz
#tar cvzf $f.tar.gz $f
#mv $f.tar.gz ~/Dropbox/Public/
rm -rf ~/gs_build/$f
mv $f ~/gs_build/
