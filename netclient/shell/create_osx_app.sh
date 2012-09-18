ZIPNAME=$1

if [ -z "$ZIPNAME" ]
then
    ZIPNAME="Gnomescroll.zip"
fi

rm -rf `pwd`/Gnomescroll.app/

cd ~/Desktop
xcodebuild -scheme "Gnomescroll Client" archive || (echo "BUILD FAILED" && exit 1)
#/Users/maslow/dc_mmo/netclient/shell/clean_awesomium_framework.sh `pwd`/Gnomescroll.app/
zip -r "$ZIPNAME" Gnomescroll.app/
mv "./$ZIPNAME" ~/Dropbox/Public/
