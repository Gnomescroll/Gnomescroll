ZIPNAME=$1

if [ -z "$ZIPNAME" ]
then
    ZIPNAME="Gnomescroll.zip"
fi

cd ~/Desktop
xcodebuild -scheme "Gnomescroll Client" archive
zip -r "$ZIPNAME" Gnomescroll.app/
mv "./$ZIPNAME" ~/Dropbox/Public/
