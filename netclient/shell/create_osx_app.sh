ZIPNAME=$1

if [ -z "$ZIPNAME" ]
then
    ZIPNAME="Gnomescroll.zip"
fi

if [ -d ~/Desktop/"$ZIPNAME"/ ]; then
    rm ~/Desktop/"$ZIPNAME"
fi

if [ -d ~/Desktop/Gnomescroll.app/ ]; then
    rm -rf ~/Desktop/Gnomescroll.app/
fi

rm -rf `pwd`/Gnomescroll.app/

cd ~/dc_mmo/src
xcodebuild -scheme "Gnomescroll Client" archive || (echo "BUILD FAILED" && exit 1)
mv ./Gnomescroll.app ~/Desktop
cd ~/Desktop
zip -r "$ZIPNAME" Gnomescroll.app/
