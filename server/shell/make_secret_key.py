#!/usr/bin/env python

import os
import binascii

key = binascii.hexlify(os.urandom(64))
print key
