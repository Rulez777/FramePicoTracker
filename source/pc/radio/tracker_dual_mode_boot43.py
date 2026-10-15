# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Narrow revision06 startup-counter repair; preserves revision05 builder.

Offline builder only. Does not authorize OTA or claim physical button timing.
"""
import argparse,json,hashlib
from pathlib import Path
from . import tracker_dual_mode_patch as original


def assembly():
    text=original_assembly()
    before='strb r3,[r2]\nmsr APSR_nzcvq,r1\npop {r0,r1,r2,r3}\npush.w'
    after=('strb r3,[r2]\nldr r2, =0x20001ff2\nstrh r3,[r2]\n'
           'msr APSR_nzcvq,r1\npop {r0,r1,r2,r3}\npush.w')
    if text.count(before)!=1:raise ValueError('Unexpected revision05 startup entry')
    return text.replace(before,after)


original_assembly=original.assembly

def build(stock,directory):
    original.assembly=assembly
    try:image,manifest=original.build(stock,directory)
    finally:original.assembly=original_assembly
    manifest.update(name='fw189-dual-mode-revision06-counter-reset',
                    revision=6,button_counter_reset_each_entry=True,
                    allowed_frame_reset_reasons=[0],
                    parent_fork_sha256='395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f',
                    revision_builder_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
    manifest['edits']=[dict(x,reason=('Clear mode and native hold counter at every startup-button entry'
                       if x['va']=='0xc3ec' else x['reason'])) for x in manifest['edits']]
    (Path(directory)/'patch-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
    assert original.reverse(image,manifest)==stock
    return image,manifest


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('stock',type=Path)
    parser.add_argument('--out-dir',type=Path,required=True);args=parser.parse_args()
    image,manifest=build(args.stock.read_bytes(),args.out_dir)
    print(json.dumps(dict(bytes=len(image),sha256=manifest['patched_sha256'],approved_for_ota=False)))
