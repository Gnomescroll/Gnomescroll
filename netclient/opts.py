#!/usr/bin/python

'''
Usage:

In main loading file (gameloop.py):

import opts
import args_client
opts.opts = args_client.get_args()

Do this *before* importing anything that will evaluate 'import opts'.


In the other files, you:

import opts
opts = opts.opts
if opts.verbose:
    print 'debug'

Do NOT do:
from opts import opts

If values in the opts.opts object change,
an opts objects imported this way will not be updated

You CANT do:
import opts.opts as opts

because opts.opts is not a module
'''

opts = None
