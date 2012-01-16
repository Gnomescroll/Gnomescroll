#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006-2010 (ita)
# Ralf Habacker, 2006 (rh)
# Yinon Ehrlich, 2009
# Michael Kuhn, 2009

from waflib.Tools import ccroot, ar
from waflib.Configure import conf

@conf
def find_xlc(conf):
	"""
	Detect the Aix C compiler
	"""
	cc = conf.find_program(['xlc_r', 'xlc'], var='CC')
	cc = conf.cmd_to_list(cc)
	conf.env.CC_NAME = 'xlc'
	conf.env.CC      = cc

@conf
def xlc_common_flags(conf):
	"""
	Flags required for executing the Aix C compiler
	"""
	v = conf.env

	v['CC_SRC_F']            = []
	v['CC_TGT_F']            = ['-c', '-o']

	# linker
	if not v['LINK_CC']: v['LINK_CC'] = v['CC']
	v['CCLNK_SRC_F']         = []
	v['CCLNK_TGT_F']         = ['-o']
	v['CPPPATH_ST']          = '-I%s'
	v['DEFINES_ST']          = '-D%s'

	v['LIB_ST']              = '-l%s' # template for adding libs
	v['LIBPATH_ST']          = '-L%s' # template for adding libpaths
	v['STLIB_ST']            = '-l%s'
	v['STLIBPATH_ST']        = '-L%s'
	v['RPATH_ST']            = '-Wl,-rpath,%s'

	v['SONAME_ST']           = []
	v['SHLIB_MARKER']        = []
	v['STLIB_MARKER']        = []

	# program
	v['LINKFLAGS_cprogram']  = ['-Wl,-brtl']
	v['cprogram_PATTERN']    = '%s'

	# shared library
	v['CFLAGS_cshlib']       = ['-fPIC']
	v['LINKFLAGS_cshlib']    = ['-G', '-Wl,-brtl,-bexpfull']
	v['cshlib_PATTERN']      = 'lib%s.so'

	# static lib
	v['LINKFLAGS_cstlib']    = []
	v['cstlib_PATTERN']      = 'lib%s.a'

def configure(conf):
	conf.find_xlc()
	conf.find_ar()
	conf.xlc_common_flags()
	conf.cc_load_tools()
	conf.cc_add_flags()
	conf.link_add_flags()

