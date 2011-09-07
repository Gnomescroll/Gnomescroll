#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2010 (ita)

"""
Various configuration tests.
"""

from waflib.Configure import conf
from waflib.TaskGen import feature, before_method
import sys

LIB_CODE = '''
#ifdef _MSC_VER
#define testEXPORT __declspec(dllexport)
#else
#define testEXPORT
#endif
testEXPORT int lib_func(void) { return 9; }
'''

MAIN_CODE = '''
#ifdef _MSC_VER
#define testEXPORT __declspec(dllimport)
#else
#define testEXPORT
#endif
testEXPORT int lib_func(void);
int main(void) {return !(lib_func() == 9);}
'''

@feature('link_lib_test')
@before_method('process_source')
def link_lib_test_fun(self):
	"""
	The configuration test :py:func:`waflib.Tools.ccroot.run_c_code` declares a unique task generator,
	so we need to create other task generators from here to check if the linker is able to link libraries.
	"""
	def write_test_file(task):
		task.outputs[0].write(task.generator.code)

	rpath = []
	if getattr(self, 'add_rpath', False):
		rpath = [self.bld.path.get_bld().abspath()]

	mode = self.mode
	m = '%s %s' % (mode, mode)
	bld = self.bld
	bld(rule=write_test_file, target='test.' + mode, code=LIB_CODE)
	bld(rule=write_test_file, target='main.' + mode, code=MAIN_CODE)
	bld(features= m + 'shlib', source='test.' + mode, target='test')
	bld(features= m + 'program test_exec', source='main.' + mode, target='app', use='test', rpath=rpath)

@conf
def check_library(self, mode=None):
	"""
	Check if libraries can be linked with the current linker. Uses :py:func:`waflib.Tools.c_tests.link_lib_test_fun`.

	:param mode: c or cxx or d
	:type mode: string
	"""
	if not mode:
		mode = 'c'
		if self.env.CXX:
			mode = 'cxx'
	self.check(
		compile_filename = [],
		features = 'link_lib_test',
		msg = 'Checking for libraries',
		mode = mode
		)

########################################################################################

INLINE_CODE = '''
typedef int foo_t;
static %s foo_t static_foo () {return 0; }
%s foo_t foo () {
	return 0;
}
'''
INLINE_VALUES = ['inline', '__inline__', '__inline']

@conf
def check_inline(self, **kw):
	"""
	Check for the right value for inline macro.
	Define INLINE_MACRO to 1 if the define is found.
	If the inline macro is not 'inline', add a define to the ``config.h`` (#define inline __inline__)

	:param define_name: define INLINE_MACRO by default to 1 if the macro is defined
	:type define_name: string
	:param features: by default *c* or *cxx* depending on the compiler present
	:type features: list of string
	"""

	self.start_msg('Checking for inline')

	if not 'define_name' in kw:
		kw['define_name'] = 'INLINE_MACRO'
	if not 'features' in kw:
		if self.env.CXX:
			kw['features'] = ['cxx']
		else:
			kw['features'] = ['c']

	for x in INLINE_VALUES:
		kw['fragment'] = INLINE_CODE % (x, x)

		try:
			self.check(**kw)
		except self.errors.ConfigurationError:
			continue
		else:
			self.end_msg(x)
			if x != 'inline':
				self.define('inline', x, quote=False)
			return x
	self.fatal('could not use inline functions')

########################################################################################

LARGE_FRAGMENT = '#include <unistd.h>\nint main() { return !(sizeof(off_t) >= 8); }\n'

@conf
def check_large_file(self, **kw):
	"""
	Check for large file support and define the macro HAVE_LARGEFILE
	The test is skipped on win32 systems (DEST_BINFMT == pe).

	:param define_name: define to set, by default *HAVE_LARGEFILE*
	:type define_name: string
	:param execute: execute the test (yes by default)
	:type execute: bool
	"""

	if not 'define_name' in kw:
		kw['define_name'] = 'HAVE_LARGEFILE'
	if not 'execute' in kw:
		kw['execute'] = True

	if not 'features' in kw:
		if self.env.CXX:
			kw['features'] = ['cxx', 'cxxprogram']
		else:
			kw['features'] = ['c', 'cprogram']

	kw['fragment'] = LARGE_FRAGMENT

	kw['msg'] = 'Checking for large file support'
	ret = True
	try:
		if self.env.DEST_BINFMT != 'pe':
			ret = self.check(**kw)
	except self.errors.ConfigurationError:
		pass
	else:
		if ret:
			return True

	kw['msg'] = 'Checking for -D_FILE_OFFSET_BITS=64'
	kw['defines'] = ['_FILE_OFFSET_BITS=64']
	try:
		ret = self.check(**kw)
	except self.errors.ConfigurationError:
		pass
	else:
		self.define('_FILE_OFFSET_BITS', 64)
		return ret

	self.fatal('There is no support for large files')

########################################################################################


