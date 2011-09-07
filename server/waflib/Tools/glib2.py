#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006-2010 (ita)

"""
Support for GLib2 tools:

* marshal
* enums
* gsettings
"""

import os
from waflib import Task, Utils, Options, Errors, Logs
from waflib.TaskGen import taskgen_method, before_method, after_method, feature

################## marshal files

@taskgen_method
def add_marshal_file(self, filename, prefix):
	"""
	Add a file to the list of marshal files to process. Store them in the attribute *marshal_list*.

	:param filename: xml file to compile
	:type filename: string
	:param prefix: marshal prefix (--prefix=prefix)
	:type prefix: string
	"""
	if not hasattr(self, 'marshal_list'):
		self.marshal_list = []
	self.meths.append('process_marshal')
	self.marshal_list.append((filename, prefix))

@before_method('process_source')
def process_marshal(self):
	"""
	Process the marshal files stored in the attribute *marshal_list* to create :py:class:`waflib.Tools.glib2.glib_genmarshal` instances.
	Add the c file created to the list of source to process.
	"""
	for f, prefix in getattr(self, 'marshal_list', []):
		node = self.path.find_resource(f)

		if not node:
			raise Errors.WafError('file not found %r' % f)

		h_node = node.change_ext('.h')
		c_node = node.change_ext('.c')

		task = self.create_task('glib_genmarshal', node, [h_node, c_node])
		task.env.GLIB_GENMARSHAL_PREFIX = prefix
	self.source = self.to_nodes(getattr(self, 'source', []))
	self.source.append(c_node)

class glib_genmarshal(Task.Task):

	def run(self):

		bld = self.inputs[0].__class__.ctx

		get = self.env.get_flat
		cmd1 = "%s %s --prefix=%s --header > %s" % (
			get('GLIB_GENMARSHAL'),
			self.inputs[0].srcpath(),
			get('GLIB_GENMARSHAL_PREFIX'),
			self.outputs[0].abspath()
		)

		ret = bld.exec_command(cmd1)
		if ret: return ret

		#print self.outputs[1].abspath()
		c = '''#include "%s"\n''' % self.outputs[0].name
		self.outputs[1].write(c)

		cmd2 = "%s %s --prefix=%s --body >> %s" % (
			get('GLIB_GENMARSHAL'),
			self.inputs[0].srcpath(),
			get('GLIB_GENMARSHAL_PREFIX'),
			self.outputs[1].abspath()
		)
		return bld.exec_command(cmd2)

	vars    = ['GLIB_GENMARSHAL_PREFIX', 'GLIB_GENMARSHAL']
	color   = 'BLUE'
	ext_out = ['.h']

########################## glib-mkenums

@taskgen_method
def add_enums_from_template(self, source='', target='', template='', comments=''):
	"""
	Add a file to the list of enum files to process. Store them in the attribute *enums_list*.

	:param source: enum file to process
	:type source: string
	:param target: target file
	:type target: string
	:param template: template file
	:type template: string
	:param comments: comments
	:type comments: string
	"""
	if not hasattr(self, 'enums_list'):
		self.enums_list = []
	self.meths.append('process_enums')
	self.enums_list.append({'source': source,
	                        'target': target,
	                        'template': template,
	                        'file-head': '',
	                        'file-prod': '',
	                        'file-tail': '',
	                        'enum-prod': '',
	                        'value-head': '',
	                        'value-prod': '',
	                        'value-tail': '',
	                        'comments': comments})

@taskgen_method
def add_enums(self, source='', target='',
              file_head='', file_prod='', file_tail='', enum_prod='',
              value_head='', value_prod='', value_tail='', comments=''):
	"""
	Add a file to the list of enum files to process. Store them in the attribute *enums_list*.

	:param source: enum file to process
	:type source: string
	:param target: target file
	:type target: string
	:param file_head: unused
	:param file_prod: unused
	:param file_tail: unused
	:param enum_prod: unused
	:param value_head: unused
	:param value_prod: unused
	:param value_tail: unused
	:param comments: comments
	:type comments: string
	"""
	if not hasattr(self, 'enums_list'):
		self.enums_list = []
	self.meths.append('process_enums')
	self.enums_list.append({'source': source,
	                        'template': '',
	                        'target': target,
	                        'file-head': file_head,
	                        'file-prod': file_prod,
	                        'file-tail': file_tail,
	                        'enum-prod': enum_prod,
	                        'value-head': value_head,
	                        'value-prod': value_prod,
	                        'value-tail': value_tail,
	                        'comments': comments})

@before_method('process_source')
def process_enums(self):
	"""
	Process the enum files stored in the attribute *enum_list* to create :py:class:`waflib.Tools.glib2.glib_mkenums` instances.
	"""
	for enum in getattr(self, 'enums_list', []):
		task = self.create_task('glib_mkenums')
		env = task.env

		inputs = []

		# process the source
		source_list = self.to_list(enum['source'])
		if not source_list:
			raise Errors.WafError('missing source ' + str(enum))
		source_list = [self.path.find_resource(k) for k in source_list]
		inputs += source_list
		env['GLIB_MKENUMS_SOURCE'] = [k.abspath() for k in source_list]

		# find the target
		if not enum['target']:
			raise Errors.WafError('missing target ' + str(enum))
		tgt_node = self.path.find_or_declare(enum['target'])
		if tgt_node.name.endswith('.c'):
			self.source.append(tgt_node)
		env['GLIB_MKENUMS_TARGET'] = tgt_node.abspath()


		options = []

		if enum['template']: # template, if provided
			template_node = self.path.find_resource(enum['template'])
			options.append('--template %s' % (template_node.abspath()))
			inputs.append(template_node)
		params = {'file-head' : '--fhead',
		           'file-prod' : '--fprod',
		           'file-tail' : '--ftail',
		           'enum-prod' : '--eprod',
		           'value-head' : '--vhead',
		           'value-prod' : '--vprod',
		           'value-tail' : '--vtail',
		           'comments': '--comments'}
		for param, option in params.items():
			if enum[param]:
				options.append('%s %r' % (option, enum[param]))

		env['GLIB_MKENUMS_OPTIONS'] = ' '.join(options)

		# update the task instance
		task.set_inputs(inputs)
		task.set_outputs(tgt_node)

class glib_mkenums(Task.Task):
	"""
	Process enum files
	"""
	run_str = '${GLIB_MKENUMS} ${GLIB_MKENUMS_OPTIONS} ${GLIB_MKENUMS_SOURCE} > ${GLIB_MKENUMS_TARGET}'
	color   = 'PINK'
	ext_out = ['.h']

######################################### gsettings

@taskgen_method
def add_settings_schemas(self, filename_list):
	"""
	Add settings files to process, add them to *settings_schema_files*

	:param filename_list: files
	:type filename_list: list of string
	"""
	if not hasattr(self, 'settings_schema_files'):
		self.settings_schema_files = []

	if not isinstance(filename_list, list):
		filename_list = [filename_list]

	self.settings_schema_files.extend(filename_list)

@taskgen_method
def add_settings_enums(self, namespace, filename_list):
	"""
	This function may be called only once by task generator to set the enums namespace.

	:param namespace: namespace
	:type namespace: string
	:param filename_list: enum files to process
	:type filename_list: file list
	"""
	if hasattr(self, 'settings_enum_namespace'):
		raise Errors.WafError("Tried to add gsettings enums to '%s' more than once" % self.name)
	self.settings_enum_namespace = namespace

	if type(filename_list) != 'list':
		filename_list = [filename_list]
	self.settings_enum_files = filename_list


def r_change_ext(self, ext):
	"""
	Change the extension from the *last* dot in the filename. The gsettings schemas
	often have names of the form org.gsettings.test.gschema.xml
	"""
	name = self.name
	k = name.rfind('.')
	if k >= 0:
		name = name[:k] + ext
	else:
		name = name + ext
	return self.parent.find_or_declare([name])

@feature('glib2')
def process_settings(self):
	"""
	Process the schema files in *settings_schema_files* to create :py:class:`waflib.Tools.glib2.glib_mkenums` instances. The
	same files are validated through :py:class:`waflib.Tools.glib2.glib_validate_schema` tasks.

	"""
	enums_tgt_node = []
	install_files = []

	settings_schema_files = getattr(self, 'settings_schema_files', [])
	if settings_schema_files and not self.env['GLIB_COMPILE_SCHEMAS']:
		raise Errors.WafError ("Unable to process GSettings schemas - glib-compile-schemas was not found during configure")

	# 1. process gsettings_enum_files (generate .enums.xml)
	#
	if hasattr(self, 'settings_enum_files'):
		enums_task = self.create_task('glib_mkenums')

		source_list = self.settings_enum_files
		source_list = [self.path.find_resource(k) for k in source_list]
		enums_task.set_inputs(source_list)
		enums_task.env['GLIB_MKENUMS_SOURCE'] = [k.abspath() for k in source_list]

		target = self.settings_enum_namespace + '.enums.xml'
		tgt_node = self.path.find_or_declare(target)
		enums_task.set_outputs(tgt_node)
		enums_task.env['GLIB_MKENUMS_TARGET'] = tgt_node.abspath()
		enums_tgt_node = [tgt_node]

		install_files.append (tgt_node)

		options = '--comments "<!-- @comment@ -->" --fhead "<schemalist>" --vhead "  <@type@ id=\\"%s.@EnumName@\\">" --vprod "    <value nick=\\"@valuenick@\\" value=\\"@valuenum@\\"/>" --vtail "  </@type@>" --ftail "</schemalist>" ' % (self.settings_enum_namespace)
		enums_task.env['GLIB_MKENUMS_OPTIONS'] = options

	# 2. process gsettings_schema_files (validate .gschema.xml files)
	#
	for schema in settings_schema_files:
		schema_task = self.create_task ('glib_validate_schema')

		schema_node = self.path.find_resource(schema)
		if not schema_node:
			raise Errors.WafError("Cannot find the schema file '%s'" % schema)
		install_files.append(schema_node)
		source_list = enums_tgt_node + [schema_node]

		schema_task.set_inputs (source_list)
		schema_task.env['GLIB_COMPILE_SCHEMAS_OPTIONS'] = [("--schema-file=" + k.abspath()) for k in source_list]

		target_node = r_change_ext (schema_node, '.xml.valid')
		schema_task.set_outputs (target_node)
		schema_task.env['GLIB_VALIDATE_SCHEMA_OUTPUT'] = target_node.abspath()

	# 3. schemas install task
	def compile_schemas_callback(bld):
		if not bld.is_install: return
		Logs.pprint ('YELLOW','Updating GSettings schema cache')
		command = Utils.subst_vars("${GLIB_COMPILE_SCHEMAS} ${GSETTINGSSCHEMADIR}", bld.env)
		ret = self.bld.exec_command(command)

	if self.bld.is_install:
		if not self.env['GSETTINGSSCHEMADIR']:
			raise Errors.WafError ('GSETTINGSSCHEMADIR not defined (should have been set up automatically during configure)')

		if install_files:
			self.bld.install_files (self.env['GSETTINGSSCHEMADIR'], install_files)

			if not hasattr(self.bld, '_compile_schemas_registered'):
				self.bld.add_post_fun (compile_schemas_callback)
				self.bld._compile_schemas_registered = True

class glib_validate_schema(Task.Task):
	"""
	Validate schema files
	"""
	run_str = 'rm -f ${GLIB_VALIDATE_SCHEMA_OUTPUT} && ${GLIB_COMPILE_SCHEMAS} --dry-run ${GLIB_COMPILE_SCHEMAS_OPTIONS} && touch ${GLIB_VALIDATE_SCHEMA_OUTPUT}'
	color   = 'PINK'

def configure(conf):
	"""
	Find the following programs:

	* *glib-genmarshal* and set *GLIB_GENMARSHAL*
	* *glib-mkenums* and set *GLIB_MKENUMS*
	* *glib-compile-schemas* and set *GLIB_COMPILE_SCHEMAS* (not mandatory)

	And set the variable *GSETTINGSSCHEMADIR*
	"""
	conf.find_program('glib-genmarshal', var='GLIB_GENMARSHAL')
	conf.find_perl_program('glib-mkenums', var='GLIB_MKENUMS')

	# when cross-compiling, gsettings.m4 locates the program with the following:
	#   pkg-config --variable glib_compile_schemas gio-2.0
	conf.find_program('glib-compile-schemas', var='GLIB_COMPILE_SCHEMAS', mandatory=False)

	def getstr(varname):
		return getattr(Options.options, varname, getattr(conf.env,varname, ''))

	# TODO make this dependent on the gnu_dirs tool?
	gsettingsschemadir = getstr('GSETTINGSSCHEMADIR')
	if not gsettingsschemadir:
		datadir = getstr('DATADIR')
		if not datadir:
			prefix = conf.env['PREFIX']
			datadir = os.path.join(prefix, 'share')
		gsettingsschemadir = os.path.join(datadir, 'glib-2.0', 'schemas')

	conf.env['GSETTINGSSCHEMADIR'] = gsettingsschemadir

def options(opt):
	"""
	Add the ``--gsettingsschemadir`` command-line option
	"""
	opt.add_option('--gsettingsschemadir', help='GSettings schema location [Default: ${datadir}/glib-2.0/schemas]',default='',dest='GSETTINGSSCHEMADIR')

