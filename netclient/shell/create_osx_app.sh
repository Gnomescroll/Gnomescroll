ZIPNAME=$1

if [ -z "$ZIPNAME" ]
then
    ZIPNAME="Gnomescroll.zip"
fi

cd ~/Desktop
xcodebuild -scheme "Gnomescroll Client" archive
clean_awesomium_framework.sh Gnomescroll.app/
zip -r "$ZIPNAME" Gnomescroll.app/
mv "./$ZIPNAME" ~/Dropbox/Public/
