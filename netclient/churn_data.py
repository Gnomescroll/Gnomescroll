#!/usr/bin/python

import commands
i = 0

with open('churnlog','w') as f:
    while True:
        if i == 0:
            s = '0000'
        else:
            s = str(i)
        cmd = "hg churn -r %s:%d" % (s, i+100,)
        print s, str(i+100)
        status, output = commands.getstatusoutput(cmd)
        if status:
            cmd = 'hg churn -r %s:' % (s,)
            newstatus, output = commands.getstatusoutput(cmd)
        f.write('Revisions %d -> %d\n' % (i, i+100))
        f.write(output)
        f.write('\n\n')
        if status: break
        i += 100
