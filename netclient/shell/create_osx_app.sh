ZIPNAME=$1

if [ -z "$ZIPNAME" ]
then
    ZIPNAME="Gnomescroll.zip"
fi

rm -rf `pwd`/Gnomescroll.app/

cd ../../src/
xcodebuild -scheme "Gnomescroll Client" archive || (echo "BUILD FAILED" && exit 1)
mv ./Gnomescroll.app ~/Desktop
cd ~/Desktop
#/Users/maslow/dc_mmo/netclient/shell/clean_awesomium_framework.sh `pwd`/Gnomescroll.app/
zip -r "$ZIPNAME" Gnomescroll.app/
mv "./$ZIPNAME" ~/Dropbox/Public/
