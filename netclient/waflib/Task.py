#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2005-2010 (ita)

"""
Tasks represent atomic operations such as processes.
"""

import os, shutil, re, tempfile
from waflib import Utils, Logs, Errors

# task states
NOT_RUN = 0
"""The task was not executed yet"""

MISSING = 1
"""The task has been executed but the files have not been created"""

CRASHED = 2
"""The task execution returned a non-zero exit status"""

EXCEPTION = 3
"""An exception occured in the task execution"""

SKIPPED = 8
"""The task did not have to be executed"""

SUCCESS = 9
"""The task was successfully executed"""

ASK_LATER = -1
"""The task is not ready to be executed"""

SKIP_ME = -2
"""The task does not need to be executed"""

RUN_ME = -3
"""The task must be executed"""

COMPILE_TEMPLATE_SHELL = '''
def f(tsk):
	env = tsk.env
	gen = tsk.generator
	bld = gen.bld
	wd = getattr(tsk, 'cwd', None)
	p = env.get_flat
	tsk.last_cmd = cmd = \'\'\' %s \'\'\' % s
	return tsk.exec_command(cmd, cwd=wd, env=env.env or None)
'''

COMPILE_TEMPLATE_NOSHELL = '''
def f(tsk):
	env = tsk.env
	gen = tsk.generator
	bld = gen.bld
	wd = getattr(tsk, 'cwd', None)
	def to_list(xx):
		if isinstance(xx, str): return [xx]
		return xx
	tsk.last_cmd = lst = []
	%s
	lst = [x for x in lst if x]
	return tsk.exec_command(lst, cwd=wd, env=env.env or None)
'''

def cache_outputs(cls):
	"""
	Task class decorator applied to all task classes by default unless they define the attribute 'nocache'::

		from waflib import Task
		class foo(Task.Task):
			nocache = True

	If bld.cache_global is defined and if the task instances produces output nodes,
	the files will be copied into a folder in the cache directory

	The files may also be retrieved from that folder, if it exists
	"""
	m1 = cls.run
	def run(self):
		bld = self.generator.bld
		if bld.cache_global and not bld.nocache:
			if self.can_retrieve_cache():
				return 0
		return m1(self)
	cls.run = run

	m2 = cls.post_run
	def post_run(self):
		bld = self.generator.bld
		ret = m2(self)
		if bld.cache_global and not bld.nocache:
			self.put_files_cache()
		return ret
	cls.post_run = post_run

	return cls


classes = {}
"class tasks created by user scripts or Waf tools are kept in this dict name -> class object"

class store_task_type(type):
	"""
	Metaclass: store the task types into :py:const:`waflib.Task.classes`.
	The attribute 'run_str' will be processed to compute a method 'run' on the task class
	The decorator :py:func:`waflib.Task.cache_outputs` is also applied to the class
	"""
	def __init__(cls, name, bases, dict):
		super(store_task_type, cls).__init__(name, bases, dict)
		name = cls.__name__

		if name.endswith('_task'):
			name = name.replace('_task', '')
		if name != 'evil' and name != 'TaskBase':
			global classes

			if getattr(cls, 'run_str', None):
				# if a string is provided, convert it to a method
				(f, dvars) = compile_fun(cls.run_str, cls.shell)
				cls.hcode = cls.run_str
				cls.run_str = None
				cls.run = f
				cls.vars = list(set(cls.vars + dvars))
				cls.vars.sort()
			elif getattr(cls, 'run', None) and not 'hcode' in cls.__dict__:
				# getattr(cls, 'hcode') would look in the upper classes
				cls.hcode = Utils.h_fun(cls.run)

			if not getattr(cls, 'nocache', None):
				cls = cache_outputs(cls)

			classes[name] = cls

evil = store_task_type('evil', (object,), {})
"Base class provided to avoid writing a metaclass, so the code can run in python 2.6 and 3.x unmodified"

class TaskBase(evil):
	"""
	Base class for all Waf tasks, which should be seen as an interface.
	For illustration purposes, instances of this class will execute the attribute
	'fun' in :py:meth:`waflib.Task.TaskBase.run`. When in doubt, create
	subclasses of :py:class:`waflib.Task.Task` instead.

	Subclasses should override these methods:

	#. __str__: string to display to the user
	#. runnable_status: ask the task if it should be run, skipped, or if we have to ask later
	#. run: let threads execute the task
	#. post_run: let threads update the data regarding the task (cache)
	"""

	color = 'GREEN'
	"""Color for the console display, see :py:const:`waflib.Logs.colors_lst`"""

	ext_in = []
	"""File extensions that objects of this task class might use"""

	ext_out = []
	"""File extensions that objects of this task class might create"""

	before = []
	"""List of task class names to execute before instances of this class"""

	after = []
	"""List of task class names to execute after instances of this class"""

	hcode = ''
	"""String representing an additional hash for the class representation"""

	def __init__(self, *k, **kw):
		"""
		The base task class requires a task generator, which will be itself if missing
		"""
		self.hasrun = NOT_RUN
		try:
			self.generator = kw['generator']
		except KeyError:
			self.generator = self

	def __repr__(self):
		"for debugging purposes"
		return '\n\t{task %r: %s %s}' % (self.__class__.__name__, id(self), str(getattr(self, 'fun', '')))

	def __str__(self):
		"string to display to the user"
		if hasattr(self, 'fun'):
			return 'executing: %s\n' % self.fun.__name__
		return self.__class__.__name__ + '\n'

	def __hash__(self):
		"Very fast hashing scheme but not persistent (replace/implement in subclasses and see :py:meth:`waflib.Task.Task.uid`)"
		return id(self)

	def exec_command(self, cmd, **kw):
		"""
		Wrapper for :py:meth:`waflib.Context.Context.exec_command` which sets a current working directory to ``build.variant_dir``

		:return: the return code
		:rtype: int
		"""
		bld = self.generator.bld
		try:
			if not kw.get('cwd', None):
				kw['cwd'] = bld.cwd
		except AttributeError:
			bld.cwd = kw['cwd'] = bld.variant_dir
		return bld.exec_command(cmd, **kw)

	def runnable_status(self):
		"""
		State of the task

		:return: a task state in :py:const:`waflib.Task.RUN_ME`, :py:const:`waflib.Task.SKIP_ME` or :py:const:`waflib.Task.ASK_LATER`.
		:rtype: int
		"""
		return RUN_ME

	def process(self):
		"""
		Assume that the task has had a new attribute ``master`` which is an instance of :py:class:`waflib.Runner.Parallel`.
		Execute the task and then put it back in the queue :py:attr:`waflib.Runner.Parallel.out` (may be replaced by subclassing).
		"""
		m = self.master
		if m.stop:
			m.out.put(self)
			return

		# remove the task signature immediately before it is executed
		# in case of failure the task will be executed again
		try:
			del self.generator.bld.task_sigs[self.uid()]
		except:
			pass

		try:
			self.generator.bld.returned_tasks.append(self)
			self.log_display(self.generator.bld)
			ret = self.run()
		except Exception:
			self.err_msg = Utils.ex_stack()
			self.hasrun = EXCEPTION

			# TODO cleanup
			m.error_handler(self)
			m.out.put(self)
			return

		if ret:
			self.err_code = ret
			self.hasrun = CRASHED
		else:
			try:
				self.post_run()
			except Errors.WafError:
				pass
			except Exception:
				self.err_msg = Utils.ex_stack()
				self.hasrun = EXCEPTION
			else:
				self.hasrun = SUCCESS
		if self.hasrun != SUCCESS:
			m.error_handler(self)

		m.out.put(self)

	def run(self):
		"""
		Execute the task (executed by threads). Override in subclasses.

		:rtype: int
		"""
		if hasattr(self, 'fun'):
			return self.fun(self)
		return 0

	def post_run(self):
		"Update the cache files (executed by threads). Override in subclasses."
		pass

	def log_display(self, bld):
		"Write the execution status on the context logger"
		bld.to_log(self.display())

	def display(self):
		"""
		Return an execution status for the console, the progress bar, or the IDE output.

		:rtype: string
		"""
		col1 = Logs.colors(self.color)
		col2 = Logs.colors.NORMAL
		master = self.master

		def cur():
			# the current task position, computed as late as possible
			tmp = -1
			if hasattr(master, 'ready'):
				tmp -= master.ready.qsize()
			return master.processed + tmp

		if self.generator.bld.progress_bar == 1:
			return self.generator.bld.progress_line(cur(), master.total, col1, col2)

		if self.generator.bld.progress_bar == 2:
			ela = str(self.generator.bld.timer)
			try:
				ins  = ','.join([n.name for n in self.inputs])
			except AttributeError:
				ins = ''
			try:
				outs = ','.join([n.name for n in self.outputs])
			except AttributeError:
				outs = ''
			return '|Total %s|Current %s|Inputs %s|Outputs %s|Time %s|\n' % (master.total, cur(), ins, outs, ela)

		s = str(self)
		if not s:
			return None

		total = master.total
		n = len(str(total))
		fs = '[%%%dd/%%%dd] %%s%%s%%s' % (n, n)
		return fs % (cur(), total, col1, s, col2)

	def attr(self, att, default=None):
		"""
		Retrieve an attribute from the instance or from the class.

		:param att: variable name
		:type att: string
		:param default: default value
		"""
		ret = getattr(self, att, self)
		if ret is self: return getattr(self.__class__, att, default)
		return ret

	def hash_constraints(self):
		"""
		Identify a task type for all the constraints relevant for the scheduler: precedence, file production

		:return: a hash value
		:rtype: string
		"""
		cls = self.__class__
		tup = (str(cls.before), str(cls.after), str(cls.ext_in), str(cls.ext_out), cls.__name__, cls.hcode)
		h = hash(tup)
		return h

	def format_error(self):
		"""
		Error message to display to the user when a build fails

		:rtype: string
		"""
		msg = getattr(self, 'last_cmd', '')
		if getattr(self, "err_msg", None):
			return self.err_msg
		elif self.hasrun == CRASHED:
			try:
				return ' -> task failed (exit status %r): %r\n%r' % (self.err_code, self, msg)
			except AttributeError:
				return ' -> task failed: %r\n%r' % (self, msg)
		elif self.hasrun == MISSING:
			return ' -> missing files: %r\n%r' % (self, msg)
		else:
			return '?'

	def colon(self, var1, var2):
		"""
		private function for the moment

		used for scriptlet expressions such as ${FOO_ST:FOO}, for example, if
		env.FOO_ST = ['-a', '-b']
		env.FOO    = ['1', '2']
		then the result will be ['-a', '-b', '1', '-a', '-b', '2']
		"""
		tmp = self.env[var1]
		if isinstance(var2, str):
			it = self.env[var2]
		else:
			it = var2
		if isinstance(tmp, str):
			return [tmp % x for x in it]
		else:
			if Logs.verbose and not tmp and it:
				Logs.warn('Missing env variable %r for task %r (generator %r)' % (var1, self, self.generator))
			lst = []
			for y in it:
				lst.extend(tmp)
				lst.append(y)
			return lst

class Task(TaskBase):
	"""
	This class deals with the filesystem (:py:class:`waflib.Node.Node`). The method :py:class:`waflib.Task.Task.runnable_status`
	uses a hash value (from :py:class:`waflib.Task.Task.signature`) which is persistent from build to build. When the value changes,
	the task has to be executed. The method :py:class:`waflib.Task.Task.post_run` will assign the task signature to the output
	nodes (if present).
	"""
	vars = []
	"""Variables to depend on (class attribute used for :py:meth:`waflib.Task.Task.sig_vars`)"""

	shell = False
	"""Execute the command with the shell (class attribute)"""

	def __init__(self, *k, **kw):
		TaskBase.__init__(self, *k, **kw)

		self.env = kw['env']
		"""ConfigSet object (make sure to provide one)"""

		self.inputs  = []
		"""List of input nodes, which represent the files used by the task instance"""

		self.outputs = []
		"""List of output nodes, which represent the files created by the task instance"""

		self.dep_nodes = []
		"""List of additional nodes to depend on"""

		self.run_after = set([])
		"""Set of tasks that must be executed before this one"""

		# Additionally, you may define the following
		#self.dep_vars  = 'PREFIX DATADIR'

	def __str__(self):
		"string to display to the user"
		env = self.env
		src_str = ' '.join([a.nice_path(env) for a in self.inputs])
		tgt_str = ' '.join([a.nice_path(env) for a in self.outputs])
		if self.outputs: sep = ' -> '
		else: sep = ''
		return '%s: %s%s%s\n' % (self.__class__.__name__.replace('_task', ''), src_str, sep, tgt_str)

	def __repr__(self):
		"for debugging purposes"
		return "".join(['\n\t{task %r: ' % id(self), self.__class__.__name__, " ", ",".join([x.name for x in self.inputs]), " -> ", ",".join([x.name for x in self.outputs]), '}'])

	def uid(self):
		"""
		Return an identifier used to determine if tasks are up-to-date. Since the
		identifier will be stored between executions, it must be:

			- unique: no two tasks return the same value (for a given build context)
			- the same for a given task instance

		By default, the node paths, the class name, and the function are used
		as inputs to compute a hash.

		The pointer to the object (python built-in 'id') will change between build executions,
		and must be avoided in such hashes.

		:return: hash value
		:rtype: string
		"""
		try:
			return self.uid_
		except AttributeError:
			# this is not a real hot zone, but we want to avoid surprizes here
			m = Utils.md5()
			up = m.update
			up(self.__class__.__name__.encode())
			for x in self.inputs + self.outputs:
				up(x.abspath().encode())
			self.uid_ = m.digest()
			return self.uid_

	def set_inputs(self, inp):
		"""
		Append the nodes to the *inputs*

		:param inp: input nodes
		:type inp: node or list of nodes
		"""
		if isinstance(inp, list): self.inputs += inp
		else: self.inputs.append(inp)

	def set_outputs(self, out):
		"""
		Append the nodes to the *outputs*

		:param out: output nodes
		:type out: node or list of nodes
		"""
		if isinstance(out, list): self.outputs += out
		else: self.outputs.append(out)

	def set_run_after(self, task):
		"""
		Run this task only after *task*. Affect :py:meth:`waflib.Task.runnable_status`

		:param task: task
		:type task: :py:class:`waflib.Task.Task`
		"""
		# TODO: handle lists too?
		assert isinstance(task, TaskBase)
		self.run_after.add(task)

	def signature(self):
		"""
		Task signatures are stored between build executions, they are use to track the changes
		made to the input nodes (not to the outputs!). The signature hashes data from various sources:

		* explicit dependencies: files listed in the inputs (list of node objects) :py:meth:`waflib.Task.Task.sig_explicit_deps`
		* implicit dependencies: list of nodes returned by scanner methods (when present) :py:meth:`waflib.Task.Task.sig_implicit_deps`
		* hashed data: variables/values read from task.__class__.vars/task.env :py:meth:`waflib.Task.Task.sig_vars`

		If the signature is expected to give a different result, clear the cache kept in ``self.cache_sig``::

			from waflib import Task
			class cls(Task.Task):
				def signature(self):
					sig = super(Task.Task, self).signature()
					delattr(self, 'cache_sig')
					return super(Task.Task, self).signature()
		"""
		try: return self.cache_sig
		except AttributeError: pass

		self.m = Utils.md5()
		self.m.update(self.hcode.encode())

		# explicit deps
		self.sig_explicit_deps()

		# env vars
		self.sig_vars()

		# implicit deps / scanner results
		if self.scan:
			try:
				self.sig_implicit_deps()
			except Errors.TaskRescan:
				return self.signature()

		ret = self.cache_sig = self.m.digest()
		return ret

	def runnable_status(self):
		"""
		Override :py:meth:`waflib.Task.TaskBase.runnable_status` to determine if the task is ready
		to be run (:py:attr:`waflib.Task.Task.run_after`)
		"""
		#return 0 # benchmarking

		for t in self.run_after:
			if not t.hasrun:
				return ASK_LATER

		bld = self.generator.bld

		# first compute the signature
		try:
			new_sig = self.signature()
		except Errors.TaskNotReady:
			return ASK_LATER

		# compare the signature to a signature computed previously
		key = self.uid()
		try:
			prev_sig = bld.task_sigs[key]
		except KeyError:
			Logs.debug("task: task %r must run as it was never run before or the task code changed" % self)
			return RUN_ME

		# compare the signatures of the outputs
		for node in self.outputs:
			try:
				if node.sig != new_sig:
					return RUN_ME
			except AttributeError:
				Logs.debug("task: task %r must run as the output nodes do not exist" % self)
				return RUN_ME

		if new_sig != prev_sig:
			return RUN_ME
		return SKIP_ME

	def post_run(self):
		"""
		Called after successful execution to update the cache data :py:class:`waflib.Node.Node` sigs
		and :py:attr:`waflib.Build.BuildContext.task_sigs`.

		The node signature is obtained from the task signature, but the output nodes may also get the signature
		of their contents. See the class decorator :py:func:`waflib.Task.update_outputs` if you need this behaviour.
		"""
		bld = self.generator.bld
		sig = self.signature()

		for node in self.outputs:
			# check if the node exists ..
			try:
				os.stat(node.abspath())
			except OSError:
				self.hasrun = MISSING
				self.err_msg = '-> missing file: %r' % node.abspath()
				raise Errors.WafError(self.err_msg)

			# important, store the signature for the next run
			node.sig = sig

		bld.task_sigs[self.uid()] = self.cache_sig

	def sig_explicit_deps(self):
		"""
		Used by :py:meth:`waflib.Task.Task.signature`, hash :py:attr:`waflib.Task.Task.inputs`
		and :py:attr:`waflib.Task.Task.dep_nodes` signatures.

		:rtype: hash value
		"""
		bld = self.generator.bld
		upd = self.m.update

		# the inputs
		for x in self.inputs + self.dep_nodes:
			try:
				upd(x.get_bld_sig())
			except (AttributeError, TypeError):
				raise Errors.WafError('Missing node signature for %r (required by %r)' % (x, self))

		# manual dependencies, they can slow down the builds
		if bld.deps_man:
			additional_deps = bld.deps_man
			for x in self.inputs + self.outputs:
				try:
					d = additional_deps[id(x)]
				except KeyError:
					continue

				for v in d:
					if isinstance(v, bld.root.__class__):
						try:
							v = v.get_bld_sig()
						except AttributeError:
							raise Errors.WafError('Missing node signature for %r (required by %r)' % (v, self))
					elif hasattr(v, '__call__'):
						v = v() # dependency is a function, call it
					upd(v)

		return self.m.digest()

	def sig_vars(self):
		"""
		Used by :py:meth:`waflib.Task.Task.signature`, hash :py:attr:`waflib.Task.Task.env` variables/values

		:rtype: hash value
		"""
		bld = self.generator.bld
		env = self.env
		upd = self.m.update

		# dependencies on the environment vars
		act_sig = bld.hash_env_vars(env, self.__class__.vars)
		upd(act_sig)

		# additional variable dependencies, if provided
		dep_vars = getattr(self, 'dep_vars', None)
		if dep_vars:
			upd(bld.hash_env_vars(env, dep_vars))

		return self.m.digest()

	scan = None
	"""
	This method, when provided, returns a tuple containing:

	* a list of nodes corresponding to real files
	* a list of names for files not found in path_lst

	For example::

		from waflib.Task import Task
		class mytask(Task):
			def scan(self, node):
				return ((), ())

	The first and second lists are stored in :py:attr:`waflib.Build.BuildContext.node_deps` and
	:py:attr:`waflib.Build.BuildContext.raw_deps` respectively.
	"""

	def sig_implicit_deps(self):
		"""
		Used by :py:meth:`waflib.Task.Task.signature` hashes node signatures obtained by scanning for dependencies (:py:meth:`waflib.Task.Task.scan`).

		The exception :py:class:`waflib.Errors.TaskRescan` is thrown
		when a file has changed. When this occurs, :py:meth:`waflib.Task.Task.signature` is called
		once again, and this method will be executed once again, this time calling :py:meth:`waflib.Task.Task.scan`
		for searching the dependencies.

		:rtype: hash value
		"""

		bld = self.generator.bld

		# get the task signatures from previous runs
		key = self.uid()
		prev = bld.task_sigs.get((key, 'imp'), [])

		# for issue #379
		if prev:
			try:
				if prev == self.compute_sig_implicit_deps():
					return prev
			except:
				# when a file was renamed (IOError usually), remove the stale nodes (headers in folders without source files)
				# this will break the order calculation for headers created during the build in the source directory (should be uncommon)
				# the behaviour will differ when top != out
				for x in bld.node_deps.get(self.uid(), []):
					if x.is_child_of(bld.srcnode):
						try:
							os.stat(x.abspath())
						except:
							try:
								del x.parent.children[x.name]
							except:
								pass
			del bld.task_sigs[(key, 'imp')]
			raise Errors.TaskRescan('rescan')

		# no previous run or the signature of the dependencies has changed, rescan the dependencies
		(nodes, names) = self.scan()
		if Logs.verbose:
			Logs.debug('deps: scanner for %s returned %s %s' % (str(self), str(nodes), str(names)))

		# store the dependencies in the cache
		bld.node_deps[key] = nodes
		bld.raw_deps[key] = names

		# might happen
		self.are_implicit_nodes_ready()

		# recompute the signature and return it
		try:
			bld.task_sigs[(key, 'imp')] = sig = self.compute_sig_implicit_deps()
		except:
			if Logs.verbose:
				for k in bld.node_deps.get(self.uid(), []):
					try:
						k.get_bld_sig()
					except:
						Logs.warn('Missing signature for node %r (may cause rebuilds)' % k)
		else:
			return sig

	def compute_sig_implicit_deps(self):
		"""
		Used by :py:meth:`waflib.Task.Task.sig_implicit_deps` for computing the actual hash of the
		:py:class:`waflib.Node.Node` returned by the scanner.

		:return: hash value
		:rtype: string
		"""

		upd = self.m.update

		bld = self.generator.bld

		self.are_implicit_nodes_ready()

		# scanner returns a node that does not have a signature
		# just *ignore* the error and let them figure out from the compiler output
		# waf -k behaviour
		for k in bld.node_deps.get(self.uid(), []):
			upd(k.get_bld_sig())
		return self.m.digest()

	def are_implicit_nodes_ready(self):
		"""
		For each node returned by the scanner, see if there is a task behind it, and force the build order

		The performance impact on null builds is nearly invisible (1.66s->1.86s), but this is due to
		agressive caching (1.86s->28s)
		"""
		bld = self.generator.bld
		try:
			cache = bld.dct_implicit_nodes
		except:
			bld.dct_implicit_nodes = cache = {}

		try:
			dct = cache[bld.cur]
		except KeyError:
			dct = cache[bld.cur] = {}
			for tsk in bld.cur_tasks:
				for x in tsk.outputs:
					dct[x] = tsk

		modified = False
		for x in bld.node_deps.get(self.uid(), []):
			if x in dct:
				self.run_after.add(dct[x])
				modified = True

		if modified:
			for tsk in self.run_after:
				if not tsk.hasrun:
					#print "task is not ready..."
					raise Errors.TaskNotReady('not ready')

	def can_retrieve_cache(self):
		"""
		Used by :py:meth:`waflib.Task.cache_outputs`

		Retrieve build nodes from the cache
		update the file timestamps to help cleaning the least used entries from the cache
		additionally, set an attribute 'cached' to avoid re-creating the same cache files

		Suppose there are files in `cache/dir1/file1` and `cache/dir2/file2`:

		#. read the timestamp of dir1
		#. try to copy the files
		#. look at the timestamp again, if it has changed, the data may have been corrupt (cache update by another process)
		#. should an exception occur, ignore the data
		"""

		if not getattr(self, 'outputs', None):
			return None

		sig = self.signature()
		ssig = Utils.to_hex(self.uid()) + Utils.to_hex(sig)

		# first try to access the cache folder for the task
		dname = os.path.join(self.generator.bld.cache_global, ssig)
		try:
			t1 = os.stat(dname).st_mtime
		except OSError:
			return None

		for node in self.outputs:
			orig = os.path.join(dname, node.name)
			try:
				shutil.copy2(orig, node.abspath())
				# mark the cache file as used recently (modified)
				os.utime(orig, None)
			except (OSError, IOError):
				Logs.debug('task: failed retrieving file')
				return None

		# is it the same folder?
		try:
			t2 = os.stat(dname).st_mtime
		except OSError:
			return None

		if t1 != t2:
			return None

		for node in self.outputs:
			node.sig = sig
			if self.generator.bld.progress_bar < 1:
				self.generator.bld.to_log('restoring from cache %r\n' % node.abspath())

		self.cached = True
		return True

	def put_files_cache(self):
		"""
		Used by :py:func:`waflib.Task.cache_outputs` to store the build files in the cache
		"""

		# file caching, if possible
		# try to avoid data corruption as much as possible
		if getattr(self, 'cached', None):
			return None
		if not getattr(self, 'outputs', None):
			return None

		sig = self.signature()
		ssig = Utils.to_hex(self.uid()) + Utils.to_hex(sig)
		dname = os.path.join(self.generator.bld.cache_global, ssig)
		tmpdir = tempfile.mkdtemp(prefix=self.generator.bld.cache_global + os.sep + 'waf')

		try:
			shutil.rmtree(dname)
		except:
			pass

		try:
			for node in self.outputs:
				dest = os.path.join(tmpdir, node.name)
				shutil.copy2(node.abspath(), dest)
		except (OSError, IOError):
			try:
				shutil.rmtree(tmpdir)
			except:
				pass
		else:
			try:
				os.rename(tmpdir, dname)
			except OSError:
				try:
					shutil.rmtree(tmpdir)
				except:
					pass
			else:
				try:
					os.chmod(dname, Utils.O755)
				except:
					pass

def is_before(t1, t2):
	"""
	Return a non-zero value if task t1 is to be executed before task t2::

		t1.ext_out = '.h'
		t2.ext_in = '.h'
		t2.after = ['t1']
		t1.before = ['t2']
		waflib.Task.is_before(t1, t2) # True

	:param t1: task
	:type t1: :py:class:`waflib.Task.TaskBase`
	:param t2: task
	:type t2: :py:class:`waflib.Task.TaskBase`
	"""
	to_list = Utils.to_list
	for k in to_list(t2.ext_in):
		if k in to_list(t1.ext_out):
			return 1

	if t1.__class__.__name__ in to_list(t2.after):
		return 1

	if t2.__class__.__name__ in to_list(t1.before):
		return 1

	return 0

def set_file_constraints(tasks):
	"""
	Adds tasks to the task 'run_after' attribute based on the task inputs and outputs

	:param tasks: tasks
	:type tasks: list of :py:class:`waflib.Task.TaskBase`
	"""
	ins = Utils.defaultdict(set)
	outs = Utils.defaultdict(set)
	for x in tasks:
		for a in getattr(x, 'inputs', []) + getattr(x, 'dep_nodes', []):
			ins[id(a)].add(x)
		for a in getattr(x, 'outputs', []):
			outs[id(a)].add(x)

	links = set(ins.keys()).intersection(outs.keys())
	for k in links:
		for a in ins[k]:
			a.run_after.update(outs[k])

def set_precedence_constraints(tasks):
	"""
	Add tasks to the task 'run_after' attribute based on the after/before/ext_out/ext_in attributes

	:param tasks: tasks
	:type tasks: list of :py:class:`waflib.Task.TaskBase`
	"""
	cstr_groups = Utils.defaultdict(list)
	for x in tasks:
		h = x.hash_constraints()
		cstr_groups[h].append(x)

	keys = list(cstr_groups.keys())
	maxi = len(keys)

	# this list should be short
	for i in range(maxi):
		t1 = cstr_groups[keys[i]][0]
		for j in range(i + 1, maxi):
			t2 = cstr_groups[keys[j]][0]

			# add the constraints based on the comparisons
			if is_before(t1, t2):
				a = i
				b = j
			elif is_before(t2, t1):
				a = j
				b = i
			else:
				continue
			for x in cstr_groups[keys[b]]:
				x.run_after.update(cstr_groups[keys[a]])

def funex(c):
	"""
	Compile a function by 'exec'

	:param c: function to compile
	:type c: string
	:return: the function 'f' declared in the input string
	:rtype: function
	"""
	dc = {}
	exec(c, dc)
	return dc['f']

reg_act = re.compile(r"(?P<backslash>\\)|(?P<dollar>\$\$)|(?P<subst>\$\{(?P<var>\w+)(?P<code>.*?)\})", re.M)
def compile_fun_shell(line):
	"""
	Create a compiled function to execute a process with the shell
	WARNING: this method may disappear anytime, so use compile_fun instead
	"""

	extr = []
	def repl(match):
		g = match.group
		if g('dollar'): return "$"
		elif g('backslash'): return '\\\\'
		elif g('subst'): extr.append((g('var'), g('code'))); return "%s"
		return None

	line = reg_act.sub(repl, line) or line

	parm = []
	dvars = []
	app = parm.append
	for (var, meth) in extr:
		if var == 'SRC':
			if meth: app('tsk.inputs%s' % meth)
			else: app('" ".join([a.path_from(bld.bldnode) for a in tsk.inputs])')
		elif var == 'TGT':
			if meth: app('tsk.outputs%s' % meth)
			else: app('" ".join([a.path_from(bld.bldnode) for a in tsk.outputs])')
		elif meth:
			if meth.startswith(':'):
				m = meth[1:]
				if m == 'SRC':
					m = '[a.path_from(bld.bldnode) for a in tsk.inputs]'
				elif m == 'TGT':
					m = '[a.path_from(bld.bldnode) for a in tsk.outputs]'
				elif m[:3] not in ('tsk', 'gen', 'bld'):
					dvars.extend([var, meth[1:]])
					m = '%r' % m
				app('" ".join(tsk.colon(%r, %s))' % (var, m))
			else:
				app('%s%s' % (var, meth))
		else:
			if not var in dvars: dvars.append(var)
			app("p('%s')" % var)
	if parm: parm = "%% (%s) " % (',\n\t\t'.join(parm))
	else: parm = ''

	c = COMPILE_TEMPLATE_SHELL % (line, parm)

	Logs.debug('action: %s' % c)
	return (funex(c), dvars)

def compile_fun_noshell(line):
	"""
	Create a compiled function to execute a process without the shell
	WARNING: this method may disappear anytime, so use compile_fun instead
	"""
	extr = []
	def repl(match):
		g = match.group
		if g('dollar'): return "$"
		elif g('subst'): extr.append((g('var'), g('code'))); return "<<|@|>>"
		return None

	line2 = reg_act.sub(repl, line)
	params = line2.split('<<|@|>>')
	assert(extr)

	buf = []
	dvars = []
	app = buf.append
	for x in range(len(extr)):
		params[x] = params[x].strip()
		if params[x]:
			app("lst.extend(%r)" % params[x].split())
		(var, meth) = extr[x]
		if var == 'SRC':
			if meth: app('lst.append(tsk.inputs%s)' % meth)
			else: app("lst.extend([a.path_from(bld.bldnode) for a in tsk.inputs])")
		elif var == 'TGT':
			if meth: app('lst.append(tsk.outputs%s)' % meth)
			else: app("lst.extend([a.path_from(bld.bldnode) for a in tsk.outputs])")
		elif meth:
			if meth.startswith(':'):
				m = meth[1:]
				if m == 'SRC':
					m = '[a.path_from(bld.bldnode) for a in tsk.inputs]'
				elif m == 'TGT':
					m = '[a.path_from(bld.bldnode) for a in tsk.outputs]'
				elif m[:3] not in ('tsk', 'gen', 'bld'):
					dvars.extend([var, m])
					m = '%r' % m
				app('lst.extend(tsk.colon(%r, %s))' % (var, m))
			else:
				app('lst.extend(gen.to_list(%s%s))' % (var, meth))
		else:
			app('lst.extend(to_list(env[%r]))' % var)
			if not var in dvars: dvars.append(var)

	if extr:
		if params[-1]:
			app("lst.extend(%r)" % params[-1].split())
	fun = COMPILE_TEMPLATE_NOSHELL % "\n\t".join(buf)
	Logs.debug('action: %s' % fun)
	return (funex(fun), dvars)

def compile_fun(line, shell=False):
	"""
	Parse a string expression such as "${CC} ${SRC} -o ${TGT}" and return a pair containing:

	* the function created (compiled) for use as :py:meth:`waflib.Task.TaskBase.run`
	* the list of variables that imply a dependency from self.env

	for example::

		from waflib.Task import compile_fun
		compile_fun('cxx', '${CXX} -o ${TGT[0]} ${SRC} -I ${SRC[0].parent.bldpath()}')

		def build(bld):
			bld(source='wscript', rule='echo "foo\\${SRC[0].name}\\bar"')

	The env variables (CXX, ..) on the task must not hold dicts (order)
	The reserved keywords *TGT* and *SRC* represent the task input and output nodes

	"""
	if line.find('<') > 0 or line.find('>') > 0 or line.find('&&') > 0:
		shell = True

	if shell:
		return compile_fun_shell(line)
	else:
		return compile_fun_noshell(line)

def task_factory(name, func=None, vars=None, color='GREEN', ext_in=[], ext_out=[], before=[], after=[], shell=False, scan=None):
	"""
	Return a new task subclass with the function ``run`` compiled from the line given.
	Provided for compatibility with waf 1.4-1.5, when we did not use metaclasses to register new objects.

	:param func: method run
	:type func: string or function
	:param vars: list of variables to hash
	:type vars: list of string
	:param color: color to use
	:type color: string
	:param shell: when *func* is a string, enable/disable the use of the shell
	:type shell: bool
	:param scan: method scan
	:type scan: function
	:rtype: :py:class:`waflib.Task.Task`
	"""

	params = {
		'vars': vars or [], # function arguments are static, and this one may be modified by the class
		'color': color,
		'name': name,
		'ext_in': Utils.to_list(ext_in),
		'ext_out': Utils.to_list(ext_out),
		'before': Utils.to_list(before),
		'after': Utils.to_list(after),
		'shell': shell,
		'scan': scan,
	}

	if isinstance(func, str):
		params['run_str'] = func
	else:
		params['run'] = func

	cls = type(Task)(name, (Task,), params)
	global classes
	classes[name] = cls
	return cls


def always_run(cls):
	"""
	Task class decorator

	Set all task instances of this class to be executed whenever a build is started
	The task signature is calculated, but the result of the comparation between
	task signatures is bypassed
	"""
	old = cls.runnable_status
	def always(self):
		ret = old(self)
		if ret == SKIP_ME:
			ret = RUN_ME
		return ret
	cls.runnable_status = always
	return cls

def update_outputs(cls):
	"""
	Task class decorator

	If you want to create files in the source directory. For example, to keep *foo.txt* in the source
	directory, create it first and declare::

		def build(bld):
			bld(rule='cp ${SRC} ${TGT}', source='wscript', target='foo.txt', update_outputs=True)
	"""
	old_post_run = cls.post_run
	def post_run(self):
		old_post_run(self)
		for node in self.outputs:
			node.sig = Utils.h_file(node.abspath())
			self.generator.bld.task_sigs[node.abspath()] = self.uid() # issue #1017
	cls.post_run = post_run


	old_runnable_status = cls.runnable_status
	def runnable_status(self):
		status = old_runnable_status(self)
		if status != RUN_ME:
			return status

		try:
			# by default, we check that the output nodes have the signature of the task
			# perform a second check, returning 'SKIP_ME' as we are expecting that
			# the signatures do not match
			bld = self.generator.bld
			prev_sig = bld.task_sigs[self.uid()]
			if prev_sig == self.signature():
				for x in self.outputs:
					if not x.sig or bld.task_sigs[x.abspath()] != self.uid():
						return RUN_ME
				return SKIP_ME
		except KeyError:
			pass
		except IndexError:
			pass
		except AttributeError:
			pass
		return RUN_ME
	cls.runnable_status = runnable_status

	return cls


