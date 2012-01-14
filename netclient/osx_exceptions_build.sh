echo 'configuring waf exceptions'

./waf exceptions

cd ./build/c4che/
echo 'hacking _cache.py with regex'
sed -ibak 's/i386/x86_64/g' _cache.py

cd ..
cd ..
./waf build
