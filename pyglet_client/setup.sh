
#echo "Running ./libsetup.sh"
#./libsetup.sh

cd server/
echo 'building server/*.py'
python setup.py build_ext --inplace $@

#cd ../netclient
#echo 'building netclient/*.py'
#python setup.py build_ext --inplace $@

cd ../admin_client
echo 'building admin_client/setup.py'
python setup.py build_ext --inplace $@

#cd ../netclient
#echo 'building netclient/setup.py -v'
#python setup.py build_ext --inplace $@

cd ../netclient
echo 'building netclient libs'
./waf -j 4 build
