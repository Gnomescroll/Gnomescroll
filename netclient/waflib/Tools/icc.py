#!/usr/bin/env python
# encoding: utf-8
# Stian Selnes, 2008
# Thomas Nagy 2009-2010 (ita)

"""
Detect the Intel C compiler
"""

import os, sys
from waflib.Tools import ccroot, ar, gcc
from waflib.Configure import conf

@conf
def find_icc(conf):
	"""
	Find the program icc and execute it to ensure it really is icc
	"""
	if sys.platform == 'cygwin':
		conf.fatal('The Intel compiler does not work on Cygwin')

	v = conf.env
	cc = None
	if v['CC']: cc = v['CC']
	elif 'CC' in conf.environ: cc = conf.environ['CC']
	if not cc: cc = conf.find_program('icc', var='CC')
	if not cc: cc = conf.find_program('ICL', var='CC')
	if not cc: conf.fatal('Intel C Compiler (icc) was not found')
	cc = conf.cmd_to_list(cc)

	conf.get_cc_version(cc, icc=True)
	v['CC'] = cc
	v['CC_NAME'] = 'icc'

def configure(conf):
	conf.find_icc()
	conf.find_ar()
	conf.gcc_common_flags()
	conf.gcc_modifier_platform()
	conf.cc_load_tools()
	conf.cc_add_flags()
	conf.link_add_flags()
