# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Direct Frame diagnostic API client over passwordless SSH; no open network port."""
import argparse
import json
from pathlib import Path
import subprocess
import time


def request(host, path, *, post=False, output=None):
    # Paths are produced locally; no shell metacharacters or arbitrary commands.
    if any(c not in '/abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-' for c in path):
        raise ValueError('Invalid API path')
    command = 'curl -fsS ' + ('-X POST ' if post else '') + 'http://127.0.0.1:18741' + path
    if output:
        with Path(output).open('wb') as stream:
            subprocess.run(['ssh','-o','BatchMode=yes',host,command],stdout=stream,check=True)
        return None
    result = subprocess.run(['ssh','-o','BatchMode=yes',host,command],capture_output=True,check=True)
    return json.loads(result.stdout)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action',choices=('status','capture','dataset'))
    parser.add_argument('--host',default='steamos@OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED')
    parser.add_argument('--out',type=Path)
    parser.add_argument('--dataset')
    parser.add_argument('--file')
    args = parser.parse_args()
    if args.action == 'status':
        print(json.dumps(request(args.host,'/status'),indent=2)); return
    if not args.out: parser.error('--out is required')
    if args.action == 'dataset':
        if not args.dataset or not args.file: parser.error('--dataset and --file required')
        request(args.host,f'/dataset/{args.dataset}/{args.file}',output=args.out); return
    identifier = request(args.host,'/capture',post=True)['id']
    print('capture',identifier,flush=True)
    while True:
        job = request(args.host,'/capture/'+identifier)
        if job['state'] in ('complete','failed'): break
        time.sleep(1)
    print(json.dumps({k:v for k,v in job.items() if k != 'manifest'},indent=2))
    if 'manifest' in job:
        print(json.dumps({k:v for k,v in job['manifest'].items() if not k.startswith('datasets_')},indent=2))
    if job['state'] != 'complete': raise RuntimeError('Capture failed; inspect restoration before retry')
    request(args.host,'/capture/'+identifier+'/bundle',output=args.out)


if __name__ == '__main__': main()
