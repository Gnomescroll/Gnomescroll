cd ~/Desktop/Gnomescroll.app/Contents/Frameworks
rm libGLEW.1.7.dylib
cp /usr/local/lib/libGLEW.1.7.0.dylib .
chown maslow:staff libGLEW.1.7.0.dylib
chmod 755 libGLEW.1.7.0.dylib
cd ../MacOS
install_name_tool -change "/usr/local/lib/libGLEW.1.7.0.dylib" "@loader_path/../Frameworks/libGLEW.1.7.0.dylib" Gnomescroll
install_name_tool -change "/usr/local/lib/libluajit-51.2.dylib" "@loader_path/../Frameworks/libluajit-51.2.dylib" Gnomescroll
