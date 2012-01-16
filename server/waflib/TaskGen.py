#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2005-2010 (ita)

"""
Task generators

The class :py:class:`waflib.TaskGen.task_gen` encapsulates the creation of task objects (low-level code)
The instances can have various parameters, but the creation of task nodes (Task.py)
is always postponed. To achieve this, various methods are called from the method "apply"


"""

import copy, re
from waflib import Task, Utils, Logs, Errors, ConfigSet

feats = Utils.defaultdict(set)
"""remember the methods declaring features"""

class task_gen(object):
	"""
	Instances of this class create :py:class:`waflib.Task.TaskBase` when
	calling the method :py:meth:`waflib.TaskGen.task_gen.post` from the main thread.
	A few notes:

	* The methods to call (*self.meths*) can be specified dynamically (removing, adding, ..)
	* The 'features' are used to add methods to self.meths and then execute them
	* The attribute 'path' is a node representing the location of the task generator
	* The tasks created are added to the attribute *tasks*
	* The attribute 'idx' is a counter of task generators in the same path
	"""

	mappings = {}
	prec = Utils.defaultdict(list)

	def __init__(self, *k, **kw):
		"""
		The task generator objects predefine various attributes (source, target) for possible
		processing by process_rule (make-like rules) or process_source (extensions, misc methods)

		The tasks are stored on the attribute 'tasks'. They are created by calling methods
		listed in self.meths *or* referenced in the attribute features
		A topological sort is performed to ease the method re-use.

		The extra key/value elements passed in kw are set as attributes
		"""

		# so we will have to play with directed acyclic graphs
		# detect cycles, etc
		self.source = ''
		self.target = ''

		self.meths = []
		"""
		List of method names to execute (it is usually a good idea to avoid touching this)
		"""

		self.prec = Utils.defaultdict(list)
		"""
		Precedence table for sorting the methods in self.meths
		"""

		self.mappings = {}
		"""
		List of mappings {extension -> function} for processing files by extension
		"""

		self.features = []
		"""
		List of feature names for bringing new methods in
		"""

		self.tasks = []
		"""
		List of tasks created.
		"""

		if not 'bld' in kw:
			# task generators without a build context :-/
			self.env = ConfigSet.ConfigSet()
			self.idx = 0
			self.path = None
		else:
			self.bld = kw['bld']
			self.env = self.bld.env.derive()
			self.path = self.bld.path # emulate chdir when reading scripts

			# provide a unique id
			try:
				self.idx = self.bld.idx[id(self.path)] = self.bld.idx.get(id(self.path), 0) + 1
			except AttributeError:
				self.bld.idx = {}
				self.idx = self.bld.idx[id(self.path)] = 1

		for key, val in kw.items():
			setattr(self, key, val)

	def __str__(self):
		"""for debugging purposes"""
		return "<task_gen %r declared in %s>" % (self.name, self.path.abspath())

	def __repr__(self):
		"""for debugging purposes"""
		lst = []
		for x in self.__dict__.keys():
			if x not in ['env', 'bld', 'compiled_tasks', 'tasks']:
				lst.append("%s=%s" % (x, repr(getattr(self, x))))
		return "bld(%s) in %s" % (", ".join(lst), self.path.abspath())

	def get_name(self):
		"""
		If not set, the name is computed from the target name::

			def build(bld):
				x = bld(name='foo')
				x.get_name() # foo
				y = bld(target='bar')
				y.get_name() # bar

		:rtype: string
		:return: name of this task generator
		"""
		try:
			return self._name
		except AttributeError:
			if isinstance(self.target, list):
				lst = [str(x) for x in self.target]
				name = self._name = ','.join(lst)
			else:
				name = self._name = str(self.target)
			return name
	def set_name(self, name):
		self._name = name

	name = property(get_name, set_name)

	def to_list(self, val):
		"""
		Ensure that a parameter is a list

		:type val: string or list of string
		:param val: input to return as a list
		:rtype: list
		"""
		if isinstance(val, str): return val.split()
		else: return val

	def post(self):
		"""
		Create task objects. The following operations are performed:

		#. The body of this method is called only once and sets the attribute ``posted``
		#. The attribute ``features`` is used to add more methods in ``self.meths``
		#. The methods are sorted by the precedence table ``self.prec`` or `:waflib:attr:waflib.TaskGen.task_gen.prec`
		#. The methods are then executed in order
		#. The tasks created are added to :py:attr:`waflib.TaskGen.task_gen.tasks`
		"""

		# we could add a decorator to let the task run once, but then python 2.3 will be difficult to support
		if getattr(self, 'posted', None):
			#error("OBJECT ALREADY POSTED" + str( self))
			return False
		self.posted = True

		keys = set(self.meths)

		# add the methods listed in the features
		self.features = Utils.to_list(self.features)
		for x in self.features + ['*']:
			st = feats[x]
			if not st:
				if not x in Task.classes:
					Logs.warn('feature %r does not exist - bind at least one method to it' % x)
			keys.update(list(st)) # ironpython 2.7 wants the cast to list

		# copy the precedence table
		prec = {}
		prec_tbl = self.prec or task_gen.prec
		for x in prec_tbl:
			if x in keys:
				prec[x] = prec_tbl[x]

		# elements disconnected
		tmp = []
		for a in keys:
			for x in prec.values():
				if a in x: break
			else:
				tmp.append(a)

		# TODO waf 1.7
		#tmp.sort()

		# topological sort
		out = []
		while tmp:
			e = tmp.pop()
			if e in keys: out.append(e)
			try:
				nlst = prec[e]
			except KeyError:
				pass
			else:
				del prec[e]
				for x in nlst:
					for y in prec:
						if x in prec[y]:
							break
					else:
						tmp.append(x)

		if prec:
			raise Errors.WafError('Cycle detected in the method execution %r' % prec)
		out.reverse()
		self.meths = out

		# then we run the methods in order
		Logs.debug('task_gen: posting %s %d' % (self, id(self)))
		for x in out:
			try:
				v = getattr(self, x)
			except AttributeError:
				raise Errors.WafError('%r is not a valid task generator method' % x)
			Logs.debug('task_gen: -> %s (%d)' % (x, id(self)))
			v()

		Logs.debug('task_gen: posted %s' % self.name)
		return True

	def get_hook(self, node):
		"""
		:param node: Input file to process
		:type node: :py:class:`waflib.Tools.Node.Node`
		:return: A method able to process the input node by looking at the extension
		:rtype: function
		"""
		name = node.name
		for k in self.mappings:
			if name.endswith(k):
				return self.mappings[k]
		for k in task_gen.mappings:
			if name.endswith(k):
				return task_gen.mappings[k]
		raise Errors.WafError("File %r has no mapping in %r (did you forget to load a waf tool?)" % (node, task_gen.mappings.keys()))

	def create_task(self, name, src=None, tgt=None):
		"""
		Wrapper for creating task objects easily

		:param name: task class name
		:type name: string
		:param src: input nodes
		:type src: list of :py:class:`waflib.Tools.Node.Node`
		:param tgt: output nodes
		:type tgt: list of :py:class:`waflib.Tools.Node.Node`
		:return: A task object
		:rtype: :py:class:`waflib.Task.TaskBase`
		"""
		task = Task.classes[name](env=self.env.derive(), generator=self)
		if src:
			task.set_inputs(src)
		if tgt:
			task.set_outputs(tgt)
		self.tasks.append(task)
		return task

	def clone(self, env):
		"""
		Make a copy of a task generator. Once the copy is made, it is necessary to ensure that the
		task generator does not create the same output files as the original, or the same files may
		be compiled twice.

		:param env: A configuration set
		:type env: :py:class:`waflib.ConfigSet.ConfigSet`
		:return: A copy
		:rtype: :py:class:`waflib.TaskGen.task_gen`
		"""
		newobj = self.bld()
		for x in self.__dict__:
			if x in ['env', 'bld']:
				continue
			elif x in ['path', 'features']:
				setattr(newobj, x, getattr(self, x))
			else:
				setattr(newobj, x, copy.copy(getattr(self, x)))

		newobj.posted = False
		if isinstance(env, str):
			newobj.env = self.bld.all_envs[env].derive()
		else:
			newobj.env = env.derive()

		return newobj

def declare_chain(name='', rule=None, reentrant=True, color='BLUE',
	ext_in=[], ext_out=[], before=[], after=[], decider=None, scan=None, install_path=None, shell=False):
	"""
	Create a new mapping and a task class for processing files by extension.
	See Tools/flex.py for an example.

	:param name: name for the task class
	:type name: string
	:param rule: function to execute or string to be compiled in a function
	:type rule: string or function
	:param reentrant: re-inject the output file in the process
	:type reentrant: bool
	:param color: color for the task output
	:type color: string
	:param ext_in: execute the task only after the files of such extensions are created
	:type ext_in: list of string
	:param ext_out: execute the task only before files of such extensions are processed
	:type ext_out: list of string
	:param before: execute instances of this task before classes of the given names
	:type before: list of string
	:param after: execute instances of this task after classes of the given names
	:type after: list of string
	:param decider: if present, use it to create the output nodes for the task
	:type decider: function
	:param scan: scanner function for the task
	:type scan: function
	:param install_path: installation path for the output nodes
	:type install_path: string
	"""
	ext_in = Utils.to_list(ext_in)
	ext_out = Utils.to_list(ext_out)
	if not name:
		name = rule
	cls = Task.task_factory(name, rule, color=color, ext_in=ext_in, ext_out=ext_out, before=before, after=after, scan=scan, shell=shell)

	def x_file(self, node):
		ext = decider and decider(self, node) or cls.ext_out
		if ext_in:
			_ext_in = ext_in[0]
		out_source = [node.change_ext(x, ext_in=_ext_in) for x in ext]
		if reentrant:
			for i in range(reentrant):
				self.source.append(out_source[i])
		tsk = self.create_task(name, node, out_source)
		if install_path:
			self.bld.install_files(install_path, out_source)
		return tsk

	for x in cls.ext_in:
		task_gen.mappings[x] = x_file
	return x_file

def taskgen_method(func):
	"""
	Decorator: register a method as a task generator method.
	The function must accept a task generator as first parameter::

		from waflib.TaskGen import taskgen_method
		@taskgen_method
		def mymethod(self):
			pass

	:param func: task generator method to add
	:type func: function
	:rtype: function
	"""
	setattr(task_gen, func.__name__, func)
	return func

def feature(*k):
	"""
	Decorator: register a task generator method that will be executed when the
	object attribute 'feature' contains the corresponding key(s)::

		from waflib.Task import feature
		@feature('myfeature')
		def myfunction(self):
			print('that is my feature!')
		def build(bld):
			bld(features='myfeature')

	:param k: feature names
	:type k: list of string
	"""
	def deco(func):
		setattr(task_gen, func.__name__, func)
		for name in k:
			feats[name].update([func.__name__])
		return func
	return deco

def before_method(*k):
	"""
	Decorator: register a task generator method which will be executed
	before the functions of given name(s)::

		from waflib.TaskGen import feature, before
		@feature('myfeature')
		@before_method('fun2')
		def fun1(self):
			print('feature 1!')
		@feature('myfeature')
		def fun2(self):
			print('feature 2!')
		def build(bld):
			bld(features='myfeature')

	:param k: method names
	:type k: list of string
	"""
	def deco(func):
		setattr(task_gen, func.__name__, func)
		for fun_name in k:
			if not func.__name__ in task_gen.prec[fun_name]:
				task_gen.prec[fun_name].append(func.__name__)
				#task_gen.prec[fun_name].sort()
		return func
	return deco
before = before_method

def after_method(*k):
	"""
	Decorator: register a task generator method which will be executed
	after the functions of given name(s)::

		from waflib.TaskGen import feature, after
		@feature('myfeature')
		@after_method('fun2')
		def fun1(self):
			print('feature 1!')
		@feature('myfeature')
		def fun2(self):
			print('feature 2!')
		def build(bld):
			bld(features='myfeature')

	:param k: method names
	:type k: list of string
	"""
	def deco(func):
		setattr(task_gen, func.__name__, func)
		for fun_name in k:
			if not fun_name in task_gen.prec[func.__name__]:
				task_gen.prec[func.__name__].append(fun_name)
				#task_gen.prec[func.__name__].sort()
		return func
	return deco
after = after_method

def extension(*k):
	"""
	Decorator: register a task generator method which will be invoked during
	the processing of source files for the extension given::

		from waflib import Task
		class mytask(Task):
			run_str = 'cp ${SRC} ${TGT}'
		@extension('.moo')
		def create_maa_file(self, node):
			self.create_task('mytask', node, node.change_ext('.maa'))
		def build(bld):
			bld(source='foo.moo')
	"""
	def deco(func):
		setattr(task_gen, func.__name__, func)
		for x in k:
			task_gen.mappings[x] = func
		return func
	return deco

# ---------------------------------------------------------------
# The following methods are task generator methods commonly used
# they are almost examples, the rest of waf core does not depend on them

@taskgen_method
def to_nodes(self, lst, path=None):
	"""
	Convert the input list into a list of nodes.
	It is used by :py:func:`waflib.TaskGen.process_source` and :py:func:`waflib.TaskGen.process_rule`.
	It is designed for source files, for folders, see :py:func:`waflib.Tools.ccroot.to_incnodes`:

	:param lst: input list
	:type lst: list of string and nodes
	:param path: path from which to search the nodes (by default, :py:attr:`waflib.TaskGen.task_gen.path`)
	:type path: :py:class:`waflib.Tools.Node.Node`
	:rtype: list of :py:class:`waflib.Tools.Node.Node`
	"""
	tmp = []
	path = path or self.path
	find = path.find_resource

	if isinstance(lst, self.path.__class__):
		lst = [lst]

	# either a list or a string, convert to a list of nodes
	for x in Utils.to_list(lst):
		if isinstance(x, str):
			node = find(x)
			if not node:
				raise Errors.WafError("source not found: %r in %r" % (x, self))
		else:
			node = x
		tmp.append(node)
	return tmp

@feature('*')
def process_source(self):
	"""
	Process each element in the attribute ``source`` by extension.

	#. The *source* list is converted through :py:meth:`waflib.TaskGen.to_nodes` to a list of :py:class:`waflib.Node.Node` first.
	#. File extensions are mapped to methods having the signature: ``def meth(self, node)`` by :py:meth:`waflib.TaskGen.extension`
	#. The method is retrieved through :py:meth:`waflib.TaskGen.task_gen.get_hook`
	#. When called, the methods may modify self.source to append more source to process
	#. The mappings can map an extension or a filename (see the code below)
	"""
	self.source = self.to_nodes(getattr(self, 'source', []))
	for node in self.source:
		self.get_hook(node)(self, node)

@feature('*')
@before_method('process_source')
def process_rule(self):
	"""
	Process the attribute ``rule``. When present, :py:meth:`waflib.TaskGen.process_source` is disabled::

		def build(bld):
			bld(rule='cp ${SRC} ${TGT}', source='wscript', target='bar.txt')
	"""
	if not getattr(self, 'rule', None):
		return

	# create the task class
	name = str(getattr(self, 'name', None) or self.target or self.rule)
	cls = Task.task_factory(name, self.rule,
		getattr(self, 'vars', []),
		shell=getattr(self, 'shell', True), color=getattr(self, 'color', 'BLUE'))

	# now create one instance
	tsk = self.create_task(name)

	if getattr(self, 'target', None):
		if isinstance(self.target, str):
			self.target = self.target.split()
		if not isinstance(self.target, list):
			self.target = [self.target]
		for x in self.target:
			if isinstance(x, str):
				tsk.outputs.append(self.path.find_or_declare(x))
			else:
				x.parent.mkdir() # if a node was given, create the required folders
				tsk.outputs.append(x)
		if getattr(self, 'install_path', None):
			# from waf 1.5
			# although convenient, it does not 1. allow to name the target file and 2. symlinks
			# TODO remove in waf 1.7
			self.bld.install_files(self.install_path, tsk.outputs)

	if getattr(self, 'source', None):
		tsk.inputs = self.to_nodes(self.source)
		# bypass the execution of process_source by setting the source to an empty list
		self.source = []

	if getattr(self, 'scan', None):
		cls.scan = self.scan

	if getattr(self, 'cwd', None):
		tsk.cwd = self.cwd

	# TODO remove on_results in waf 1.7
	if getattr(self, 'update_outputs', None) or getattr(self, 'on_results', None):
		Task.update_outputs(cls)

	if getattr(self, 'always', None):
		Task.always_run(cls)

	for x in ['after', 'before', 'ext_in', 'ext_out']:
		setattr(cls, x, getattr(self, x, []))

@feature('seq')
def sequence_order(self):
	"""
	Add a strict sequential constraint between the tasks generated by task generators.
	It works because task generators are posted in order.
	It will not post objects which belong to other folders.

	Example::

		bld(features='javac seq')
		bld(features='jar seq')

	To start a new sequence, set the attribute seq_start, for example::

		obj = bld(features='seq')
		obj.seq_start = True

	Note that the method is executed in last position. This is more an
	example than a widely-used solution.
	"""
	if self.meths and self.meths[-1] != 'sequence_order':
		self.meths.append('sequence_order')
		return

	if getattr(self, 'seq_start', None):
		return

	# all the tasks previously declared must be run before these
	if getattr(self.bld, 'prev', None):
		self.bld.prev.post()
		for x in self.bld.prev.tasks:
			for y in self.tasks:
				y.set_run_after(x)

	self.bld.prev = self


re_m4 = re.compile('@(\w+)@', re.M)

class subst_pc(Task.Task):
	"""
	Create *.pc* files from *.pc.in*. The task is executed whenever an input variable used
	in the substitution changes.
	"""

	def run(self):
		"Substitutes variables in a .in file"

		code = self.inputs[0].read()

		# replace all % by %% to prevent errors by % signs
		code = code.replace('%', '%%')

		# extract the vars foo into lst and replace @foo@ by %(foo)s
		lst = []
		def repl(match):
			g = match.group
			if g(1):
				lst.append(g(1))
				return "%%(%s)s" % g(1)
			return ''
		code = re_m4.sub(repl, code)

		try:
			d = self.generator.dct
		except AttributeError:
			d = {}
			for x in lst:
				tmp = getattr(self.generator, x, '') or self.env.get_flat(x) or self.env.get_flat(x.upper())
				d[x] = str(tmp)

		self.outputs[0].write(code % d)
		self.generator.bld.raw_deps[self.uid()] = self.dep_vars = lst

		# make sure the signature is updated
		try: delattr(self, 'cache_sig')
		except AttributeError: pass

	def sig_vars(self):
		"""
		Compute a hash (signature) of the variables used in the substitution
		"""
		bld = self.generator.bld
		env = self.env
		upd = self.m.update

		# raw_deps: persistent custom values returned by the scanner
		vars = self.generator.bld.raw_deps.get(self.uid(), [])

		# hash both env vars and task generator attributes
		act_sig = bld.hash_env_vars(env, vars)
		upd(act_sig)

		lst = [getattr(self.generator, x, '') for x in vars]
		upd(Utils.h_list(lst))

		return self.m.digest()

@extension('.pc.in')
def add_pcfile(self, node):
	"""
	Process *.pc.in* files to *.pc*. Install the results to ``${PREFIX}/lib/pkgconfig/``

		def build(bld):
			bld(source='foo.pc.in', install_path='${LIBDIR}/pkgconfig/')
	"""
	tsk = self.create_task('subst_pc', node, node.change_ext('.pc', '.pc.in'))
	self.bld.install_files(getattr(self, 'install_path', '${LIBDIR}/pkgconfig/'), tsk.outputs)

class subst(subst_pc):
	pass

@feature('subst')
@before_method('process_source', 'process_rule')
def process_subst(self):
	"""
	Define a transformation that substitutes the contents of *source* files to *target* files::

		def build(bld):
			bld(
				features='subst',
				source='foo.c.in',
				target='foo.c',
				install_path='${LIBDIR}/pkgconfig',
				VAR = 'val'
			)

	The input files are supposed to contain macros of the form *@VAR@*, where *VAR* is an argument
	of the task generator object.

	This method overrides the processing by :py:meth:`waflib.TaskGen.process_source`.
	"""
	src = self.to_nodes(getattr(self, 'source', []))
	tgt = getattr(self, 'target', [])
	if isinstance(tgt, self.path.__class__):
		tgt = [tgt]
	tgt = [isinstance(x, self.path.__class__) and x or self.path.find_or_declare(x) for x in Utils.to_list(tgt)]

	if len(src) != len(tgt):
		raise Errors.WafError('invalid source or target for %r' % self)

	for x, y in zip(src, tgt):
		if not (x and y):
			raise Errors.WafError('invalid source or target for %r' % self)
		tsk = self.create_task('subst', x, y)
		for a in ('after', 'before', 'ext_in', 'ext_out'):
			val = getattr(self, a, None)
			if val:
				setattr(tsk, a, val)

	inst_to = getattr(self, 'install_path', None)
	if inst_to:
		self.bld.install_files(inst_to, tgt, chmod=getattr(self, 'chmod', Utils.O644))

	self.source = []

