#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006-2010 (ita)
# Ralf Habacker, 2006 (rh)

import os
from waflib import Utils
from waflib.Tools import ccroot, ar
from waflib.Configure import conf

@conf
def find_sxx(conf):
	"""
	Detect the sun C++ compiler
	"""
	v = conf.env
	cc = None
	if v['CXX']: cc = v['CXX']
	elif 'CXX' in conf.environ: cc = conf.environ['CXX']
	if not cc: cc = conf.find_program('CC', var='CXX') #studio
	if not cc: cc = conf.find_program('c++', var='CXX')
	if not cc: conf.fatal('Could not find a Sun C++ compiler')
	cc = conf.cmd_to_list(cc)

	try:
		conf.cmd_and_log(cc + ['-flags'])
	except:
		conf.fatal('%r is not a Sun compiler' % cc)

	v['CXX']  = cc
	v['CXX_NAME'] = 'sun'

@conf
def sxx_common_flags(conf):
	"""
	Flags required for executing the sun C++ compiler
	"""
	v = conf.env

	v['CXX_SRC_F']           = []
	v['CXX_TGT_F']           = ['-c', '-o']

	# linker
	if not v['LINK_CXX']: v['LINK_CXX'] = v['CXX']
	v['CXXLNK_SRC_F']        = []
	v['CXXLNK_TGT_F']        = ['-o']
	v['CPPPATH_ST']          = '-I%s'
	v['DEFINES_ST']          = '-D%s'

	v['LIB_ST']              = '-l%s' # template for adding libs
	v['LIBPATH_ST']          = '-L%s' # template for adding libpaths
	v['STLIB_ST']            = '-l%s'
	v['STLIBPATH_ST']        = '-L%s'

	v['SONAME_ST']           = '-Wl,-h,%s'
	v['SHLIB_MARKER']        = '-Bdynamic'
	v['STLIB_MARKER']        = '-Bstatic'

	# program
	v['cxxprogram_PATTERN']  = '%s'

	# shared library
	v['CXXFLAGS_cxxshlib']   = ['-Kpic', '-DPIC']
	v['LINKFLAGS_cxxshlib']  = ['-G']
	v['cxxshlib_PATTERN']    = 'lib%s.so'

	# static lib
	v['LINKFLAGS_cxxstlib']  = ['-Bstatic']
	v['cxxstlib_PATTERN']    = 'lib%s.a'

def configure(conf):
	conf.find_sxx()
	conf.find_ar()
	conf.sxx_common_flags()
	conf.cxx_load_tools()
	conf.cxx_add_flags()
	conf.link_add_flags()

