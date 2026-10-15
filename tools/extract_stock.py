"""Validate an independently supplied FW189 vendor container; never access hardware."""
import argparse,hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('container',type=Path);ap.add_argument('--output',type=Path);a=ap.parse_args()
 c=a.container.read_bytes()
 if len(c)!=193385 or hashlib.sha256(c).hexdigest()!='41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54':raise ValueError('Not the exact audited FW189 container')
 first=c[321:96853];second=c[96853:193385]
 if first!=second or len(first)!=96532 or hashlib.sha256(first).hexdigest()!='477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89':raise ValueError('Application copies/hash mismatch')
 if a.output:
  target=a.output.resolve()
  if target==a.container.resolve() or target.is_relative_to(ROOT):raise ValueError('Output must be a new private file outside the handoff')
  target.parent.mkdir(parents=True,exist_ok=True)
  with target.open('xb') as f:f.write(first)
 print(json.dumps({'container_verified':True,'identical_applications':True,'application_bytes':len(first),'application_sha256':hashlib.sha256(first).hexdigest(),'output_written':bool(a.output),'device_actions':False}))
if __name__=='__main__':main()
