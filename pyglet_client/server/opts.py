#!/usr/bin/python

'''
Usage:

In main loading file (gameloop.py):

import opts
import args_client
opts.opts = args_client.get_args()

Do this *before* importing anything that will evaluate the statement 'from opts import opts'.


In the other files, you can:

from opts import opts
if opts.verbose:
    print 'debug'
'''

opts = None
