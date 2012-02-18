mkdir ${1}_pkg
mkdir ${1}_pkg/lib
cd ${1}_pkg/lib
cp /usr/lib/libpython2.7.so.1.0 .
cd -
mkdir ${1}_pkg/bin
cp run ${1}_pkg/bin/run.bin
rm ${1}_pkg/bin/run
echo "export LD_LIBRARY_PATH=\"../lib\"" >> ${1}_pkg/bin/run
echo "./run.bin" >> ${1}_pkg/bin/run
chmod +x ${1}_pkg/bin/run
cp libc_lib.so build/init_c_lib.so ${1}_pkg/lib/
tar cvzf ${1}_pkg.tar.gz ${1}_pkg/
mv ${1}_pkg.tar.gz ~/Dropbox/Public/
