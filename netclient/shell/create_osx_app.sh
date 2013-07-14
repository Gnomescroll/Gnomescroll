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
zip -r "$ZIPNAME" Gnomescroll.app/
mv "./$ZIPNAME" ~/Dropbox/Public/
