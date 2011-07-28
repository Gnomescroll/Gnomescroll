
#echo "Running ./libsetup.sh"
#./libsetup.sh

cd server/
echo 'building server/*.py'
python setup.py build_ext --inplace $@

#cd ../netclient
#echo 'building netclient/*.py'
#python setup.py build_ext --inplace $@

cd ../netclient
echo 'building netclient/linux_setup.py'
python linux_setup.py build_ext --inplace $@
