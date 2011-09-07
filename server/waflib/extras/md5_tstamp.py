#! /usr/bin/env python
# encoding: utf-8

"""
Store some values on the buildcontext mapping file paths to
stat values and md5 values (timestamp + md5)
this way the md5 hashes are computed only when timestamp change (can be faster)
There is usually little or no gain from enabling this, but it can be used to enable
the second level cache with timestamps (WAFCACHE)

You may have to run distclean or to remove the build directory before enabling/disabling
this hashing scheme
"""

import os, stat
try: import cPickle
except: import pickle as cPickle
from waflib import Utils, Build, Context

Context.DBFILE += '_md5tstamp'

Build.hash_cache = {}
Build.SAVED_ATTRS.append('hash_cache')
def store(self):
	# save the hash cache as part of the default pickle file
	self.hash_cache = Build.hash_cache
	self.store_real()
Build.BuildContext.store_real = Build.BuildContext.store
Build.BuildContext.store      = store

def restore(self):
	# we need a module variable for h_file below
	self.restore_real()
	try:
		Build.hash_cache = self.hash_cache or {}
	except Exception as e:
		Build.hash_cache = {}
Build.BuildContext.restore_real = Build.BuildContext.restore
Build.BuildContext.restore      = restore

def h_file(filename):
	st = os.stat(filename)
	if stat.S_ISDIR(st[stat.ST_MODE]): raise IOError('not a file')

	if filename in Build.hash_cache:
		if Build.hash_cache[filename][0] == str(st.st_mtime):
			return Build.hash_cache[filename][1]
	m = Utils.md5()
	m.update(str(st.st_mtime))
	m.update(str(st.st_size))
	m.update(filename)
	# ensure that the cache is overwritten
	Build.hash_cache[filename] = (str(st.st_mtime), m.digest())
	return m.digest()
Utils.h_file = h_file

