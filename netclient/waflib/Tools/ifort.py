#! /usr/bin/env python
# encoding: utf-8
# DC 2008
# Thomas Nagy 2010 (ita)

import re
from waflib import Utils
from waflib.Tools import fc, fc_config, fc_scan
from waflib.Configure import conf

@conf
def find_ifort(conf):
	fc = conf.find_program('ifort', var='FC')
	fc = conf.cmd_to_list(fc)
	conf.get_ifort_version(fc)
	conf.env.FC_NAME = 'IFORT'

@conf
def ifort_modifier_cygwin(conf):
	raise NotImplementedError("Ifort on cygwin not yet implemented")

@conf
def ifort_modifier_platform(conf):
	dest_os = conf.env['DEST_OS'] or Utils.unversioned_sys_platform()
	ifort_modifier_func = getattr(conf, 'ifort_modifier_' + dest_os, None)
	if ifort_modifier_func:
		ifort_modifier_func()

@conf
def get_ifort_version(conf, fc):
	"""get the compiler version"""

	version_re = re.compile(r"ifort\s*\(IFORT\)\s*(?P<major>\d*)\.(?P<minor>\d*)", re.I).search
	cmd = fc + ['--version']
	out, err = fc_config.getoutput(conf, cmd, stdin=False)
	if out:
		match = version_re(out)
	else:
		match = version_re(err)
	if not match:
		conf.fatal('cannot determine ifort version.')
	k = match.groupdict()
	conf.env['FC_VERSION'] = (k['major'], k['minor'])

def configure(conf):
	conf.find_ifort()
	conf.find_ar()
	conf.fc_flags()
	conf.ifort_modifier_platform()
