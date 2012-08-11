ZIPNAME=$1

if [ -z "$ZIPNAME" ]
then
    ZIPNAME="Gnomescroll.zip"
fi

cd ~/Desktop
xcodebuild -scheme "Gnomescroll Client" archive
/Users/maslow/dc_mmo/netclient/shell/clean_awesomium_framework.sh `pwd`/Gnomescroll.app/
zip -r "$ZIPNAME" Gnomescroll.app/
mv "./$ZIPNAME" ~/Dropbox/Public/
