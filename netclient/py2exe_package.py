#!/c/Python27/python.exe

# setup.py
from distutils.core import setup
import py2exe
import glob

setup(console=["gameloop.py"],)

'''
      data_files=[
		("media", glob.glob("media\\*")),
		("ext", glob.glob("ext\\*")),
	  
				   ],
'''