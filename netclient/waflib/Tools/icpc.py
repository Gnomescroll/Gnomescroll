#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy 2009-2010 (ita)

"""
Detect the Intel C++ compiler
"""

import os, sys
from waflib.Tools import ccroot, ar, gxx
from waflib.Configure import conf

@conf
def find_icpc(conf):
	"""
	Find the program icpc, and execute it to ensure it really is icpc
	"""
	if sys.platform == 'cygwin':
		conf.fatal('The Intel compiler does not work on Cygwin')

	v = conf.env
	cxx = None
	if v['CXX']: cxx = v['CXX']
	elif 'CXX' in conf.environ: cxx = conf.environ['CXX']
	if not cxx: cxx = conf.find_program('icpc', var='CXX')
	if not cxx: conf.fatal('Intel C++ Compiler (icpc) was not found')
	cxx = conf.cmd_to_list(cxx)

	conf.get_cc_version(cxx, icc=True)
	v['CXX'] = cxx
	v['CXX_NAME'] = 'icc'

def configure(conf):
	conf.find_icpc()
	conf.find_ar()
	conf.gxx_common_flags()
	conf.gxx_modifier_platform()
	conf.cxx_load_tools()
	conf.cxx_add_flags()
	conf.link_add_flags()

