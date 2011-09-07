#! /usr/bin/env python
# encoding: utf-8
# DC 2008
# Thomas Nagy 2010 (ita)

"""
Fortran configuration helpers
"""

import re, shutil, os, sys, string, shlex
from waflib.Configure import conf
from waflib.TaskGen import feature, after_method, before_method
from waflib import Build, Utils

FC_FRAGMENT = '        program main\n        end     program main\n'
FC_FRAGMENT2 = '        PROGRAM MAIN\n        END\n' # what's the actual difference between these?

@conf
def fc_flags(conf):
	"""
	Define common fortran configuration flags and file extensions
	"""
	v = conf.env

	v['FC_SRC_F']    = []
	v['FC_TGT_F']    = ['-c', '-o']
	v['FCINCPATH_ST']  = '-I%s'
	v['FCDEFINES_ST']  = '-D%s'

	if not v['LINK_FC']: v['LINK_FC'] = v['FC']
	v['FCLNK_SRC_F'] = []
	v['FCLNK_TGT_F'] = ['-o']

	v['FCFLAGS_fcshlib']   = ['-fpic']
	v['LINKFLAGS_fcshlib'] = ['-shared']
	v['fcshlib_PATTERN']   = 'lib%s.so'

	v['fcstlib_PATTERN']   = 'lib%s.a'

	v['FCLIB_ST']       = '-l%s'
	v['FCLIBPATH_ST']   = '-L%s'
	v['FCSTLIB_ST']     = '-l%s'
	v['FCSTLIBPATH_ST'] = '-L%s'
	v['FCSTLIB_MARKER'] = '-Wl,-Bstatic'
	v['FCSHLIB_MARKER'] = '-Wl,-Bdynamic'

	v['SONAME_ST']           = '-Wl,-h,%s'


@conf
def check_fortran(self, *k, **kw):
	"""See if the fortran compiler works by compiling a simple fortran program"""
	self.check_cc(
		fragment         = FC_FRAGMENT,
		compile_filename = 'test.f',
		features         = 'fc fcprogram',
		msg              = 'Compiling a simple fortran app')

@conf
def check_fc(self, *k, **kw):
	"""
	Same as :py:func:`waflib.Tools.c_config.check` but default to the *Fortran* programming language
	(Overriding the C defaults in :py:func:`waflib.Tools.c_config.validate_c` here)
	"""
	kw['compiler'] = 'fc'
	if not 'compile_mode' in kw:
		kw['compile_mode'] = 'fc'
	if not 'type' in kw:
		kw['type'] = 'fcprogram'
	if not 'compile_filename' in kw:
		kw['compile_filename'] = 'test.f90'
	if not 'code' in kw:
		kw['code'] = FC_FRAGMENT
	return self.check(*k, **kw)

# ------------------------------------------------------------------------
# --- These are the default platform modifiers, refactored here for
#     convenience.  gfortran and g95 have much overlap.
# ------------------------------------------------------------------------

@conf
def fortran_modifier_darwin(conf):
	"""
	Define fortran flags and extensions for the OSX systems
	"""
	v = conf.env
	v['FCFLAGS_fcshlib']   = ['-fPIC', '-compatibility_version', '1', '-current_version', '1']
	v['LINKFLAGS_fcshlib'] = ['-dynamiclib']
	v['fcshlib_PATTERN']   = 'lib%s.dylib'
	v['FRAMEWORKPATH_ST']  = '-F%s'
	v['FRAMEWORK_ST']      = '-framework %s'

	v['LINKFLAGS_fcstlib'] = []

	v['FCSHLIB_MARKER']    = ''
	v['FCSTLIB_MARKER']    = ''
	v['SONAME_ST']         = ''


@conf
def fortran_modifier_win32(conf):
	"""Define fortran flags for the windows platforms"""
	v = conf.env
	v['fcprogram_PATTERN'] = v['fcprogram_test_PATTERN']  = '%s.exe'

	v['fcshlib_PATTERN']   = '%s.dll'
	v['implib_PATTERN']    = 'lib%s.dll.a'
	v['IMPLIB_ST']         = '-Wl,--out-implib,%s'

	v['FCFLAGS_fcshlib']   = []

	v.append_value('FCFLAGS_fcshlib', ['-DDLL_EXPORT']) # TODO adding nonstandard defines like this DLL_EXPORT is not a good idea

	# Auto-import is enabled by default even without this option,
	# but enabling it explicitly has the nice effect of suppressing the rather boring, debug-level messages
	# that the linker emits otherwise.
	v.append_value('LINKFLAGS', ['-Wl,--enable-auto-import'])

@conf
def fortran_modifier_cygwin(conf):
	"""Define fortran flags for use on cygwin"""
	fortran_modifier_win32(conf)
	v = conf.env
	v['fcshlib_PATTERN'] = 'cyg%s.dll'
	v.append_value('LINKFLAGS_fcshlib', ['-Wl,--enable-auto-image-base'])
	v['FCFLAGS_fcshlib'] = []
# ------------------------------------------------------------------------

@conf
def check_fortran_dummy_main(self, *k, **kw):
	"""
	Guess if a main function is needed by compiling a code snippet with
	the C compiler and link with the Fortran compiler

	TODO: (DC)
	- handling dialects (F77, F90, etc... -> needs core support first)
	- fix dummy main check (AC_FC_DUMMY_MAIN vs AC_FC_MAIN)

	TODO: what does the above mean? (ita)
	"""

	if not self.env.CC:
		self.fatal('A c compiler is required for check_fortran_dummy_main')

	lst = ['MAIN__', '__MAIN', '_MAIN', 'MAIN_', 'MAIN']
	lst.extend([m.lower() for m in lst])
	lst.append('')

	self.start_msg('Detecting whether we need a dummy main')
	for main in lst:
		kw['fortran_main'] = main
		try:
			self.check_cc(
				fragment = 'int %s() { return 0; }\n' % (main or 'test'),
				features = 'c fcprogram',
				mandatory = True
			)
			if not main:
				self.env.FC_MAIN = -1
				self.end_msg('no')
			else:
				self.env.FC_MAIN = main
				self.end_msg('yes %s' % main)
			break
		except self.errors.ConfigurationError:
			pass
	else:
		self.end_msg('not found')
		self.fatal('could not detect whether fortran requires a dummy main, see the config.log')

# ------------------------------------------------------------------------

GCC_DRIVER_LINE = re.compile('^Driving:')
POSIX_STATIC_EXT = re.compile('\S+\.a')
POSIX_LIB_FLAGS = re.compile('-l\S+')

@conf
def is_link_verbose(self, txt):
	"""Return True if 'useful' link options can be found in txt"""
	assert isinstance(txt, str)
	for line in txt.splitlines():
		if not GCC_DRIVER_LINE.search(line):
			if POSIX_STATIC_EXT.search(line) or POSIX_LIB_FLAGS.search(line):
				return True
	return False

@conf
def check_fortran_verbose_flag(self, *k, **kw):
	"""
	Check what kind of verbose (-v) flag works, then set it to env.FC_VERBOSE_FLAG
	"""
	self.start_msg('fortran link verbose flag')
	for x in ['-v', '--verbose', '-verbose', '-V']:
		try:
			self.check_cc(
				features = 'fc fcprogram_test',
				fragment = FC_FRAGMENT2,
				compile_filename = 'test.f',
				linkflags = [x],
				mandatory=True
				)
		except self.errors.ConfigurationError:
			pass
		else:
			# output is on stderr or stdout (for xlf)
			if self.is_link_verbose(self.test_bld.err) or self.is_link_verbose(self.test_bld.out):
				self.end_msg(x)
				break
	else:
		self.end_msg('failure')
		self.fatal('Could not obtain the fortran link verbose flag (see config.log)')

	self.env.FC_VERBOSE_FLAG = x
	return x

# ------------------------------------------------------------------------

# linkflags which match those are ignored
LINKFLAGS_IGNORED = [r'-lang*', r'-lcrt[a-zA-Z0-9\.]*\.o', r'-lc$', r'-lSystem', r'-libmil', r'-LIST:*', r'-LNO:*']
if os.name == 'nt':
	LINKFLAGS_IGNORED.extend([r'-lfrt*', r'-luser32', r'-lkernel32', r'-ladvapi32', r'-lmsvcrt', r'-lshell32', r'-lmingw', r'-lmoldname'])
else:
	LINKFLAGS_IGNORED.append(r'-lgcc*')
RLINKFLAGS_IGNORED = [re.compile(f) for f in LINKFLAGS_IGNORED]

def _match_ignore(line):
	"""Returns True if the line should be ignored (fortran test for verbosity)."""
	for i in RLINKFLAGS_IGNORED:
		if i.match(line):
			return True
	return False

def parse_fortran_link(lines):
	"""Given the output of verbose link of Fortran compiler, this returns a
	list of flags necessary for linking using the standard linker."""
	# TODO: On windows ?
	final_flags = []
	for line in lines:
		if not GCC_DRIVER_LINE.match(line):
			_parse_flink_line(line, final_flags)
	return final_flags

SPACE_OPTS = re.compile('^-[LRuYz]$')
NOSPACE_OPTS = re.compile('^-[RL]')

def _parse_flink_line(line, final_flags):
	"""private"""
	lexer = shlex.shlex(line, posix = True)
	lexer.whitespace_split = True

	t = lexer.get_token()
	tmp_flags = []
	while t:
		def parse(token):
			# Here we go (convention for wildcard is shell, not regex !)
			#   1 TODO: we first get some root .a libraries
			#   2 TODO: take everything starting by -bI:*
			#   3 Ignore the following flags: -lang* | -lcrt*.o | -lc |
			#   -lgcc* | -lSystem | -libmil | -LANG:=* | -LIST:* | -LNO:*)
			#   4 take into account -lkernel32
			#   5 For options of the kind -[[LRuYz]], as they take one argument
			#   after, the actual option is the next token
			#   6 For -YP,*: take and replace by -Larg where arg is the old
			#   argument
			#   7 For -[lLR]*: take

			# step 3
			if _match_ignore(token):
				pass
			# step 4
			elif token.startswith('-lkernel32') and sys.platform == 'cygwin':
				tmp_flags.append(token)
			# step 5
			elif SPACE_OPTS.match(token):
				t = lexer.get_token()
				if t.startswith('P,'):
					t = t[2:]
				for opt in t.split(os.pathsep):
					tmp_flags.append('-L%s' % opt)
			# step 6
			elif NOSPACE_OPTS.match(token):
				tmp_flags.append(token)
			# step 7
			elif POSIX_LIB_FLAGS.match(token):
				tmp_flags.append(token)
			else:
				# ignore anything not explicitely taken into account
				pass

			t = lexer.get_token()
			return t
		t = parse(t)

	final_flags.extend(tmp_flags)
	return final_flags

@conf
def check_fortran_clib(self, autoadd=True, *k, **kw):
	"""
	Obtain the flags for linking with the C library
	if this check works, add uselib='CLIB' to your task generators
	"""
	if not self.env.FC_VERBOSE_FLAG:
		self.fatal('env.FC_VERBOSE_FLAG is not set: execute check_fortran_verbose_flag?')

	self.start_msg('Getting fortran runtime link flags')
	try:
		self.check_cc(
			fragment = FC_FRAGMENT2,
			compile_filename = 'test.f',
			features = 'fc fcprogram_test',
			linkflags = [self.env.FC_VERBOSE_FLAG]
		)
	except:
		self.end_msg(False)
		if kw.get('mandatory', True):
			conf.fatal('Could not find the c library flags')
	else:
		out = self.test_bld.err
		flags = parse_fortran_link(out.splitlines())
		self.end_msg('ok (%s)' % ' '.join(flags))
		self.env.LINKFLAGS_CLIB = flags
		return flags
	return []

def getoutput(conf, cmd, stdin=False):
	"""
	TODO a bit redundant, can be removed anytime
	"""
	try:
		if stdin:
			stdin = Utils.subprocess.PIPE
		else:
			stdin = None
		p = Utils.subprocess.Popen(cmd, stdin=stdin, stdout=Utils.subprocess.PIPE, stderr=Utils.subprocess.PIPE)
		if stdin:
			p.stdin.write('\n'.encode())
		stdout, stderr = p.communicate()
	except:
		conf.fatal('could not determine the compiler version %r' % cmd)
	else:
		if not isinstance(stdout, str):
			stdout = stdout.decode(sys.stdout.encoding)
		if not isinstance(stderr, str):
			stderr = stderr.decode(sys.stdout.encoding)
		return stdout, stderr

# ------------------------------------------------------------------------

ROUTINES_CODE = """\
      subroutine foobar()
      return
      end
      subroutine foo_bar()
      return
      end
"""

MAIN_CODE = """
void %(dummy_func_nounder)s(void);
void %(dummy_func_under)s(void);
int %(main_func_name)s() {
  %(dummy_func_nounder)s();
  %(dummy_func_under)s();
  return 0;
}
"""

@feature('link_main_routines_func')
@before_method('process_source')
def link_main_routines_tg_method(self):
	"""
	The configuration test declares a unique task generator,
	so we create other task generators from there for fortran link tests
	"""
	def write_test_file(task):
		task.outputs[0].write(task.generator.code)
	bld = self.bld
	bld(rule=write_test_file, target='main.c', code=MAIN_CODE % self.__dict__)
	bld(rule=write_test_file, target='test.f', code=ROUTINES_CODE)
	bld(features='fc fcstlib', source='test.f', target='test')
	bld(features='c fcprogram', source='main.c', target='app', use='test')

def mangling_schemes():
	"""
	Generate triplets for use with mangle_name
	(used in check_fortran_mangling)
	the order is tuned for gfortan
	"""
	for u in ['_', '']:
		for du in ['', '_']:
			for c in ["lower", "upper"]:
				yield (u, du, c)

def mangle_name(u, du, c, name):
	"""Mangle a name from a triplet (used in check_fortran_mangling)"""
	return getattr(name, c)() + u + (name.find('_') != -1 and du or '')

@conf
def check_fortran_mangling(self, *k, **kw):
	"""
	Detect the mangling scheme, sets FORTRAN_MANGLING to the triplet found

	This test will compile a fortran static library, then link a c app against it
	"""
	if not self.env.CC:
		self.fatal('A c compiler is required for link_main_routines')
	if not self.env.FC:
		self.fatal('A fortran compiler is required for link_main_routines')
	if not self.env.FC_MAIN:
		self.fatal('Checking for mangling requires self.env.FC_MAIN (execute "check_fortran_dummy_main" first?)')

	self.start_msg('Getting fortran mangling scheme')
	for (u, du, c) in mangling_schemes():
		try:
			self.check_cc(
				compile_filename = [],
				features         = 'link_main_routines_func',
				msg = 'nomsg',
				errmsg = 'nomsg',
				mandatory=True,
				dummy_func_nounder = mangle_name(u, du, c, "foobar"),
				dummy_func_under   = mangle_name(u, du, c, "foo_bar"),
				main_func_name     = self.env.FC_MAIN
			)
		except self.errors.ConfigurationError:
			pass
		else:
			self.end_msg("ok ('%s', '%s', '%s-case')" % (u, du, c))
			self.env.FORTRAN_MANGLING = (u, du, c)
			break
	else:
		self.end_msg(False)
		self.fatal('mangler not found')

	return (u, du, c)

@feature('pyext')
@before_method('propagate_uselib_vars', 'apply_link')
def set_lib_pat(self):
	"""Set the fortran flags for linking with the python library"""
	self.env['fcshlib_PATTERN'] = self.env['pyext_PATTERN']

