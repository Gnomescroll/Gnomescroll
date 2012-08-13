#!/bin/bash

cd $1

cd ./Contents/Frameworks/Awesomium.framework/

rm -rf Versions/Current
rm Awesomium
rm Resources

ln -s Versions/A/Awesomium Awesomium
ln -s Versions/A/Resources Resources

