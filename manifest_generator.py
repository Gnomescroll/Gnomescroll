#!/usr/bin/env python

import os
import sys
from hashlib import sha256

USAGE = """Usage:
    %s <source_directory> <manifest_name> <base_url_path> <version>""" % (__file__,)
    
NARGS = 3

if len(sys.argv) < NARGS+1:
    print USAGE
    sys.exit()

source_dir = sys.argv[1]
manifest_file = sys.argv[2]
base_url_path = sys.argv[4]
version = sys.argv[3]

if not os.path.exists(source_dir):
    print "Source directory %s does not exist." % (source_dir,)
    print USAGE
    sys.exit()
    
if not os.path.isdir(source_dir):
    print "Source directory %s is not a directory" % (source_dir,)
    print USAGE
    sys.exit()

# iterate all files recursively in given directory
# printing filename hash

def get_file_hash(filename):
    assert os.path.exists(filename)
    with open(filename, 'rb') as f:
        data = f.read()
    return sha256(data).hexdigest()

def get_filenames(folder):
    assert os.path.exists(folder)
    assert os.path.isdir(folder)
    for root, dirs, files in os.walk(folder):
        for file in files:
            yield os.path.join(root, file)

def warn_and_remove_invalid_filenames(filenames):
    invalid_filenames = []
    for f in filenames:
        try:
            assert ' ' not in f         # check for spaces in names
        except AssertionError:
            invalid_filenames.append(f)
            
    if len(invalid_filenames):
        print "The following files have invalid filenames (no spaces allowed):"
        print '\n'.join(invalid_filenames)
        print ''
        
    # remove invalid names
    [filenames.remove(f) for f in invalid_filenames]

filenames = list(get_filenames(source_dir))
filenames = list(set(filenames)) # remove any duplicates just in case
warn_and_remove_invalid_filenames(filenames)
print "%d files found in %s" % (len(filenames), source_dir,)

if not len(filenames):
    print "Abort"
    exit()

hashes = [get_file_hash(f) for f in filenames]

# remove source_dir from the file path, so the path becomes relative to the source_dir
filenames = [f.replace(source_dir, '', 1) for f in filenames]

# merge filenames and hashes into "filename hash" format
manifests = zip(filenames, hashes)
manifests = [a[0] + ' ' + a[1] for a in manifests]

# write manifest
with open(manifest_file, 'w') as f:
    f.write(version)
    f.write('\n')
    f.write(base_url_path)
    f.write('\n')
    f.write('\n'.join(manifests))
    f.write('\n')

print "Created manifest %s for version %s" % (manifest_file, version,)
