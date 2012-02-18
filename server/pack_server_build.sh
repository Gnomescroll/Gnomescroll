f="SERVERBUILD"
rm -rf $f
mkdir $f/
cp -R ext $f/
cp libc_lib.so $f/
cp *.pyc $f/
cp run $f/
mkdir $f/media
cp -R ../netclient/media/vox $f/media/
cp -R ../netclient/media/voxel $f/media/
cp -R ../netclient/media/maps $f/media/
tar cvzf $f.tar.gz $f

