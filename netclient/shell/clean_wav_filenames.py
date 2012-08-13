#!/usr/bin/env python

import os

fs = os.listdir('.')
for f in fs:
    nf = f.lower().replace(' ', '_')
    if nf != f:
        os.rename(f,nf)

	
