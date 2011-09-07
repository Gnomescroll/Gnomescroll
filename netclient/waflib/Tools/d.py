#!/usr/bin/env python
# encoding: utf-8
# Carlos Rafael Giani, 2007 (dv)
# Thomas Nagy, 2007-2010 (ita)

import os, sys
from waflib import Utils, Task, Errors
from waflib.TaskGen import taskgen_method, feature, after_method, before_method, extension
from waflib.Configure import conf
from waflib.Tools.ccroot import link_task
from waflib.Tools import d_scan, d_config
from waflib.Tools.ccroot import link_task, stlink_task

class d(Task.Task):
	"Compile a d file into an object file"
	color   = 'GREEN'
	run_str = '${D} ${DFLAGS} ${DINC_ST:INCPATHS} ${D_SRC_F:SRC} ${D_TGT_F:TGT}'
	scan    = d_scan.scan

class d_with_header(d):
	"Compile a d file and generate a header"
	run_str = '${D} ${DFLAGS} ${DINC_ST:INCPATHS} ${D_HDR_F:tgt.outputs[1].bldpath()} ${D_SRC_F:SRC} ${D_TGT_F:tgt.outputs[0].bldpath()}'

class d_header(Task.Task):
	"Compile d headers"
	color   = 'BLUE'
	run_str = '${D} ${D_HEADER} ${SRC}'

class dprogram(link_task):
	"Link object files into a d program"
	run_str = '${D_LINKER} ${DLNK_SRC_F}${SRC} ${DLNK_TGT_F:TGT} ${RPATH_ST:RPATH} ${DSTLIB_MARKER} ${DSTLIBPATH_ST:STLIBPATH} ${DSTLIB_ST:STLIB} ${DSHLIB_MARKER} ${LIBPATH_ST:LIBPATH} ${DSHLIB_ST:LIB} ${LINKFLAGS}'
	inst_to = '${BINDIR}'
	chmod   = Utils.O755

class dshlib(dprogram):
	"Link object files into a d shared library"
	inst_to = '${LIBDIR}'

class dstlib(stlink_task):
	"Link object files into a d static library"
	pass # do not remove

@extension('.d', '.di', '.D')
def d_hook(self, node):
	"""
	Compile *D* files. To get .di files as well as .o files, set the following::

		def build(bld):
			bld.program(source='foo.d', target='app', generate_headers=True)

	"""
	if getattr(self, 'generate_headers', None):
		task = self.create_compiled_task('d_with_header', node)
		header_node = node.change_ext(self.env['DHEADER_ext'])
		task.outputs.append(header_node)
	else:
		task = self.create_compiled_task('d', node)
	return task

@taskgen_method
def generate_header(self, filename, install_path=None):
	"""
	See feature request #104::

		def build(bld):
			tg = bld.program(source='foo.d', target='app')
			tg.generate_header('blah.d')
			# is equivalent to:
			#tg = bld.program(source='foo.d', target='app', header_lst='blah.d')

	:param filename: header to create
	:type filename: string
	:param install_path: unused (TODO)
	"""
	try:
		self.header_lst.append([filename, install_path])
	except AttributeError:
		self.header_lst = [[filename, install_path]]

@feature('d')
def process_header(self):
	"""
	Process the attribute 'header_lst' to create the d header compilation tasks::

		def build(bld):
			bld.program(source='foo.d', target='app', header_lst='blah.d')
	"""
	for i in getattr(self, 'header_lst', []):
		node = self.path.find_resource(i[0])
		if not node:
			raise Errors.WafError('file %r not found on d obj' % i[0])
		self.create_task('d_header', node, node.change_ext('.di'))

