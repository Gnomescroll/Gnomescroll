#!/usr/bin/env python


import sys
import argparse
import os
from datetime import datetime

def get_args():

    parser = argparse.ArgumentParser(description='Clean up hourly gnomescroll backups')
    parser.add_argument('backup_path', help='Where the backups are stored')
    return parser.parse_args()

def cleanup(backup_path):
    files = os.listdir(backup_path)
    files = [f for f in files if f.startswith('world-') and f.endswith('.tar.gz')]
    
    if not files:
        print 'No files found in %s' % (backup_path,)
        return 0

    # sort them according to version
    version_bins = {}
    for f in files:
        pts = f.split('-')
        if len(pts) <= 1:
            sys.stderr.write('Unrecognized filename format: %s. [expected world-version-]\n' % (f,))
            continue
        bld = pts[1]
        version_bins.setdefault(bld, []).append(f)

    # sort them according to day
    day_bins = {}
    for version, version_files in version_bins.items():
        for f in version_files:
            pts = f.split('_')
            if len(pts) <= 1:
                sys.stderr.write('Unrecognized filename format: %s. [expected something_something*]\n' % (f,))
                continue
            dates = pts[1].split('-')
            if len(dates) < 3:
                sys.stderr.write('Unrecognized filename format: %s. [expected MM-DD-YYYY*]\n' % (f,))
                continue
            dates = '-'.join(dates[:3])
            day_bins.setdefault(dates, []).append(f)

    # skip today
    today = datetime.today().date().strftime('%m-%d-%Y')
    if today in day_bins:
        del day_bins[today]

    # reduce them
    rmfiles = []
    for date, bin in day_bins.items():
        bin.sort()
        cnt = len(bin)
        if not cnt:
            continue
        save = []
        save.append(bin.pop(-1))
        rmfiles += bin[:]
        day_bins[date] = save

    for f in rmfiles:
        os.remove(os.path.join(backup_path, f))

    return 0

if __name__ == '__main__':
    args = get_args()
    if not os.path.exists(args.backup_path):
        sys.stderr.write('Backup path %s not found\n', args.backup_path)
        sys.exit(1)
    ret = cleanup(args.backup_path)
    if not ret:
        sys.exit(ret)
