"""Offline sparse-diff verification/reconstruction. No device or network interfaces."""
import argparse,hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def sha(data):return hashlib.sha256(data).hexdigest()
def apply(stock,diff):
 if diff['format']!='pico-stock-relative-sparse-diff-v1':raise ValueError('Unsupported diff format')
 if len(stock)!=diff['stock_image_bytes'] or sha(stock)!=diff['stock_image_sha256']:raise ValueError('Wrong exact stock image/version')
 if diff['unchanged_stock_bytes_included']!=0:raise ValueError('Not a sparse difference-only record')
 out=bytearray(stock);end=0;changed=0
 for s in diff['spans']:
  off=s['offset'];before=bytes.fromhex(s['expected_stock_hex']);after=bytes.fromhex(s['replacement_hex'])
  if off<end or len(before)!=len(after) or off+len(before)>len(stock):raise ValueError('Overlapping/out-of-range edit')
  if out[off:off+len(before)]!=before:raise ValueError('Stock precondition mismatch')
  if any(a==b for a,b in zip(before,after)):raise ValueError('Unchanged stock bytes embedded in edit')
  out[off:off+len(before)]=after;end=off+len(before);changed+=len(before)
 if changed!=diff['changed_original_bytes'] or diff['append_offset']!=len(stock):raise ValueError('Diff accounting mismatch')
 out.extend(bytes.fromhex(diff['appended_authored_difference_hex']))
 if len(out)!=diff['result_image_bytes'] or sha(out)!=diff['result_image_sha256']:raise ValueError('Result hash mismatch')
 return bytes(out)
def reverse(patched,diff):
 if len(patched)!=diff['result_image_bytes'] or sha(patched)!=diff['result_image_sha256']:raise ValueError('Wrong exact patched image')
 out=bytearray(patched[:diff['stock_image_bytes']])
 for s in reversed(diff['spans']):
  off=s['offset'];before=bytes.fromhex(s['expected_stock_hex']);after=bytes.fromhex(s['replacement_hex'])
  if out[off:off+len(after)]!=after:raise ValueError('Reverse precondition mismatch')
  out[off:off+len(after)]=before
 if sha(out)!=diff['stock_image_sha256']:raise ValueError('Reverse stock hash mismatch')
 return bytes(out)
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--input',type=Path,required=True);ap.add_argument('--diff',type=Path,required=True);ap.add_argument('--reverse',action='store_true');ap.add_argument('--output',type=Path)
 args=ap.parse_args();data=args.input.read_bytes();diff=json.loads(args.diff.read_text(encoding='utf-8'))
 out=reverse(data,diff) if args.reverse else apply(data,diff)
 if not args.reverse and reverse(out,diff)!=data:raise ValueError('Round-trip mismatch')
 if args.output:
  target=args.output.resolve()
  if target==args.input.resolve() or target.is_relative_to(ROOT):raise ValueError('Refuse source overwrite or reconstructed image inside handoff')
  target.parent.mkdir(parents=True,exist_ok=True)
  with target.open('xb') as f:f.write(out)
 print(json.dumps({'verified':True,'operation':'reverse' if args.reverse else 'apply','output_written':bool(args.output),'result_bytes':len(out),'sha256':sha(out),'device_actions':False}))
if __name__=='__main__':main()
