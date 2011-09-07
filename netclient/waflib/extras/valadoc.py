#! /usr/bin/env python
# encoding: UTF-8
# Nicolas Joseph 2009

"""
ported from waf 1.5:
TODO: tabs vs spaces
"""

from waflib import Task, Utils, Node, Errors
from waflib.TaskGen import feature, extension, after_method
from Logs import debug, warn, error

VALADOC_STR = '${VALADOC}'

class valadoc(Task.Task):
	vars  = ['VALADOC', 'VALADOCFLAGS']
	color = 'BLUE'
	after = ['cprogram', 'cstlib', 'cshlib', 'cxxprogram', 'cxxstlib', 'cxxshlib']
	quiet = True # no outputs .. this is weird

	def __init__(self, *k, **kw):
		Task.Task.__init__(*k, **kw)
		self.output_dir = ''
		self.doclet = ''
		self.package_name = ''
		self.package_version = ''
		self.files = []
		self.protected = True
		self.private = False
		self.inherit = False
		self.deps = False
		self.enable_non_null_experimental = False
		self.force = False

	def run(self):
		if not self.env['VALADOCFLAGS']:
			self.env['VALADOCFLAGS'] = ''
		cmd = [Utils.subst_vars(VALADOC_STR, self.env)]
		cmd.append ('-o %s' % self.output_dir)
		if getattr(self, 'doclet', None):
			cmd.append ('--doclet %s' % self.doclet)
		cmd.append ('--package-name %s' % self.package_name)
		if getattr(self, 'version', None):
			cmd.append ('--package-version %s' % self.package_version)
		if getattr(self, 'packages', None):
			for package in self.packages:
				cmd.append ('--pkg %s' % package)
		if getattr(self, 'vapi_dirs', None):
			for vapi_dir in self.vapi_dirs:
				cmd.append ('--vapidir %s' % vapi_dir)
		if not getattr(self, 'protected', None):
			cmd.append ('--no-protected')
		if getattr(self, 'private', None):
			cmd.append ('--private')
		if getattr(self, 'inherit', None):
			cmd.append ('--inherit')
		if getattr(self, 'deps', None):
			cmd.append ('--deps')
		if getattr(self, 'enable_non_null_experimental', None):
			cmd.append ('--enable-non-null-experimental')
		if getattr(self, 'force', None):
			cmd.append ('--force')
		cmd.append (' '.join ([x.relpath_gen (self.generator.bld.bldnode) for x in self.files]))
		return self.generator.bld.exec_command(' '.join(cmd))

@feature('valadoc')
def process_valadoc(self):
	task = self.create_task('valadoc')
	if getattr(self, 'output_dir', None):
		task.output_dir = self.output_dir
	else:
		Errors.WafError('no output directory')
	if getattr(self, 'doclet', None):
		task.doclet = self.doclet
	else:
		Errors.WafError('no doclet directory')
	if getattr(self, 'package_name', None):
		task.package_name = self.package_name
	else:
		Errors.WafError('no package name')
	if getattr(self, 'package_version', None):
		task.package_version = self.package_version
	if getattr(self, 'packages', None):
		task.packages = Utils.to_list(self.packages)
	if getattr(self, 'vapi_dirs', None):
		task.vapi_dirs = Utils.to_list(self.vapi_dirs)
	if getattr(self, 'files', None):
		task.files = self.files
	else:
		Errors.WafError('no input file')
	if getattr(self, 'protected', None):
		task.protected = self.protected
	if getattr(self, 'private', None):
		task.private = self.private
	if getattr(self, 'inherit', None):
		task.inherit = self.inherit
	if getattr(self, 'deps', None):
		task.deps = self.deps
	if getattr(self, 'enable_non_null_experimental', None):
		task.enable_non_null_experimental = self.enable_non_null_experimental
	if getattr(self, 'force', None):
		task.force = self.force

def configure(conf):
	conf.find_program('valadoc', errmsg='You must install valadoc <http://live.gnome.org/Valadoc> for generate the API documentation')

