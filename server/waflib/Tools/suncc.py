#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006-2010 (ita)
# Ralf Habacker, 2006 (rh)

import os
from waflib import Utils
from waflib.Tools import ccroot, ar
from waflib.Configure import conf

@conf
def find_scc(conf):
	"""
	Detect the Sun C compiler
	"""
	v = conf.env
	cc = None
	if v['CC']: cc = v['CC']
	elif 'CC' in conf.environ: cc = conf.environ['CC']
	if not cc: cc = conf.find_program('cc', var='CC')
	if not cc: conf.fatal('Could not find a Sun C compiler')
	cc = conf.cmd_to_list(cc)

	try:
		conf.cmd_and_log(cc + ['-flags'])
	except:
		conf.fatal('%r is not a Sun compiler' % cc)

	v['CC']  = cc
	v['CC_NAME'] = 'sun'

@conf
def scc_common_flags(conf):
	"""
	Flags required for executing the sun C compiler
	"""
	v = conf.env

	v['CC_SRC_F']            = []
	v['CC_TGT_F']            = ['-c', '-o']

	# linker
	if not v['LINK_CC']: v['LINK_CC'] = v['CC']
	v['CCLNK_SRC_F']         = ''
	v['CCLNK_TGT_F']         = ['-o']
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
	v['cprogram_PATTERN']    = '%s'

	# shared library
	v['CFLAGS_cshlib']       = ['-Kpic', '-DPIC']
	v['LINKFLAGS_cshlib']    = ['-G']
	v['cshlib_PATTERN']      = 'lib%s.so'

	# static lib
	v['LINKFLAGS_cstlib']    = ['-Bstatic']
	v['cstlib_PATTERN']      = 'lib%s.a'

def configure(conf):
	conf.find_scc()
	conf.find_ar()
	conf.scc_common_flags()
	conf.cc_load_tools()
	conf.cc_add_flags()
	conf.link_add_flags()

