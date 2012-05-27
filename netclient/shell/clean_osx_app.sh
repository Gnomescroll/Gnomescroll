cd ~/Desktop/Gnomescroll.app/Contents/MacOS
install_name_tool -change "/usr/local/lib/libGLEW.1.7.0.dylib" "@loader_path/../Frameworks/libGLEW.1.7.0.dylib" Gnomescroll
install_name_tool -change "/usr/local/lib/libluajit-51.2.dylib" "@loader_path/../Frameworks/libluajit-51.2.dylib" Gnomescroll
