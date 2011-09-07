#!/usr/bin/env python
# encoding: utf-8
# andersg at 0x63.nu 2007
# Thomas Nagy 2010 (ita)

"""
Support for Perl extensions. A C/C++ compiler is required::

	def options(opt):
		opt.load('compiler_c perl')
	def configure(conf):
		conf.load('compiler_c perl')
		conf.check_perl_version((5,6,0))
		conf.check_perl_ext_devel()
		conf.check_perl_module('Cairo')
		conf.check_perl_module('Devel::PPPort 4.89')
	def build(bld):
		bld(
			features     = 'c cshlib perlext',
			source       = 'Mytest.xs',
			target       = 'Mytest',
			install_path = '${ARCHDIR_PERL}/auto')
		bld.install_files('${ARCHDIR_PERL}', 'Mytest.pm')
"""

import os
from waflib import Task, Options, Utils
from waflib.Configure import conf
from waflib.TaskGen import extension, feature, before_method

@before_method('apply_incpaths', 'apply_link', 'propagate_uselib_vars')
@feature('perlext')
def init_perlext(self):
	"""
	Change the values of *cshlib_PATTERN* and *cxxshlib_PATTERN* to remove the
	*lib* prefix from library names.
	"""
	self.uselib = self.to_list(getattr(self, 'uselib', []))
	if not 'PERLEXT' in self.uselib: self.uselib.append('PERLEXT')
	self.env['cshlib_PATTERN'] = self.env['cxxshlib_PATTERN'] = self.env['perlext_PATTERN']

@extension('.xs')
def xsubpp_file(self, node):
	"""
	Create :py:class:`waflib.Tools.perl.xsubpp` tasks to process *.xs* files
	"""
	outnode = node.change_ext('.c')
	self.create_task('xsubpp', node, outnode)
	self.source.append(outnode)

class xsubpp(Task.Task):
	"""
	Process *.xs* files
	"""
	run_str = '${PERL} ${XSUBPP} -noprototypes -typemap ${EXTUTILS_TYPEMAP} ${SRC} > ${TGT}'
	color   = 'BLUE'
	ext_out = ['.h']

@conf
def check_perl_version(self, minver=None):
	"""
	Check if Perl is installed, and set the variable PERL.
	"""
	res = True

	if not getattr(Options.options, 'perlbinary', None):
		perl = self.find_program('perl', var='PERL')
		if not perl:
			return False
	else:
		self.env['PERL'] = perl = Options.options.perlbinary

	version = self.cmd_and_log([perl, "-e", 'printf \"%vd\", $^V'])
	if not version:
		res = False
		version = "Unknown"
	elif not minver is None:
		ver = tuple(map(int, version.split(".")))
		if ver < minver:
			res = False

	if minver is None:
		cver = ""
	else:
		cver = ".".join(map(str,minver))
	self.msg('Checking for perl version', cver)
	return res

@conf
def check_perl_module(self, module):
	"""
	Check if specified perlmodule is installed.

	The minimum version can be specified by specifying it after modulename
	like this::

		def configure(conf):
			conf.check_perl_module("Some::Module 2.92")
	"""
	cmd = [self.env['PERL'], '-e', 'use %s' % module]
	self.start_msg('perl module %s' % module)
	try:
		r = self.cmd_and_log(cmd)
	except:
		self.end_msg(False)
		return None
	self.end_msg(r or True)
	return r

@conf
def check_perl_ext_devel(self):
	"""
	Check for configuration needed to build perl extensions.

	Sets different xxx_PERLEXT variables in the environment.

	Also sets the ARCHDIR_PERL variable useful as installation path,
	which can be overridden by ``--with-perl-archdir`` option.
	"""

	env = self.env
	perl = env.PERL
	if not perl:
		self.fatal('find perl first')

	def read_out(cmd):
		return Utils.to_list(self.cmd_and_log(perl + cmd))

	env['LINKFLAGS_PERLEXT'] = read_out(" -MConfig -e'print $Config{lddlflags}'")
	env['INCLUDES_PERLEXT'] = read_out(" -MConfig -e'print \"$Config{archlib}/CORE\"'")
	env['CFLAGS_PERLEXT'] = read_out(" -MConfig -e'print \"$Config{ccflags} $Config{cccdlflags}\"'")

	env['XSUBPP'] = read_out(" -MConfig -e'print \"$Config{privlib}/ExtUtils/xsubpp$Config{exe_ext}\"'")
	env['EXTUTILS_TYPEMAP'] = read_out(" -MConfig -e'print \"$Config{privlib}/ExtUtils/typemap\"'")

	if not getattr(Options.options, 'perlarchdir', None):
		env['ARCHDIR_PERL'] = self.cmd_and_log(perl + " -MConfig -e'print $Config{sitearch}'")
	else:
		env['ARCHDIR_PERL'] = getattr(Options.options, 'perlarchdir')

	env['perlext_PATTERN'] = '%s.' + self.cmd_and_log(perl + " -MConfig -e'print $Config{dlext}'")

def options(opt):
	"""
	Add the ``--with-perl-archdir`` and ``--with-perl-binary`` command-line options.
	"""
	opt.add_option('--with-perl-binary', type='string', dest='perlbinary', help = 'Specify alternate perl binary', default=None)
	opt.add_option('--with-perl-archdir', type='string', dest='perlarchdir', help = 'Specify directory where to install arch specific files', default=None)

