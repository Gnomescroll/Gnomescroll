#!/bin/bash

# DONT USE THIS

cd $1

cd ./Contents/Frameworks/Awesomium.framework/

rm -rf Versions/Current
ln -s Versions/A Versions/Current

rm Awesomium
rm Resources

ln -s Versions/Current/Awesomium Awesomium
ln -s Versions/Current/Resources Resources

