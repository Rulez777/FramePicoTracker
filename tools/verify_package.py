"""Offline audit of this text-only handoff. Does not open external files or devices."""
from pathlib import Path
import hashlib,json,re,sys
ROOT=Path(__file__).resolve().parents[1]
ALLOWED={'.md','.json','.py'}
EXEMPT={'verification/PACKAGE_VERIFICATION.json','verification/PACKAGE_MANIFEST.json'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def verify():
 errors=[];rows=[];links=0;files=sorted(p for p in ROOT.rglob('*') if p.is_file() and '.git' not in p.relative_to(ROOT).parts)
 publication_path=ROOT/'verification/PUBLICATION_ASSETS.json'
 publication_assets=json.loads(publication_path.read_text(encoding='utf-8')) if publication_path.exists() else []
 binary_allow={r['path']:r['sha256'] for r in publication_assets}
 publication_text={'LICENSE','.gitignore','.gitattributes'}
 supplement_path=ROOT/'verification/SOURCE_SUPPLEMENT.json'
 supplement=json.loads(supplement_path.read_text(encoding='utf-8')) if supplement_path.exists() else {'included':[]}
 source_allow={'source/'+r['path']:r['bundled_sha256'] for r in supplement['included']}
 state=json.loads((ROOT/'BUILD_STATUS.json').read_text(encoding='utf-8'))
 if state.get('status')!='READY':errors.append({'file':'BUILD_STATUS.json','issue':'package still building'})
 for p in files:
  rel=p.relative_to(ROOT).as_posix()
  if '__pycache__' in p.parts:errors.append({'file':rel,'issue':'compiled cache inside handoff'});continue
  if p.is_symlink() or p.is_junction():errors.append({'file':rel,'issue':'linked file'});continue
  if p.suffix.lower() not in ALLOWED and rel not in source_allow and rel not in binary_allow and rel not in publication_text:errors.append({'file':rel,'issue':'non-allowlisted file type'})
  if rel.startswith('source/') and rel!='source/README.md':
   if rel not in source_allow or sha(p)!=source_allow.get(rel):errors.append({'file':rel,'issue':'source absent from reviewed supplement manifest or hash mismatch'})
  raw=p.read_bytes()
  if rel in binary_allow:
   if sha(p)!=binary_allow[rel] or not raw.startswith(b'\x89PNG'):errors.append({'file':rel,'issue':'approved presentation asset differs'})
   rows.append({'path':rel,'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest()});continue
  if raw.startswith((b'\x7fELF',b'MZ',b'PK\x03\x04',b'\x1f\x8b',b'\x89PNG',b'\xff\xd8\xff')):errors.append({'file':rel,'issue':'binary/archive signature'})
  try:text=raw.decode('utf-8')
  except UnicodeDecodeError:errors.append({'file':rel,'issue':'not UTF-8 text'});continue
  if rel.startswith('source/') and re.search(r'3eed1979|7919ed3e|ad1da201|01a21dad',text,re.I):errors.append({'file':rel,'issue':'known private chip literal'})
  if re.search('[\u0400-\u04ff]',text):errors.append({'file':rel,'issue':'non-English Cyrillic content'})
  if re.search(r'PC[0-9A-Z]{10,}|SPSB[0-9A-Z]{6,}|\bPA[0-9][0-9A-Z]{9,}\b|192\.168\.\d+\.\d+',text):errors.append({'file':rel,'issue':'private identity/address'})
  if re.search(r'(?:chip|tracker[_ -]?id)\s*[:=]\s*(?:0x)?[0-9a-fA-F]{8}\b',text,re.I):errors.append({'file':rel,'issue':'literal device identity context'})
  if re.search(r'-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----',text):errors.append({'file':rel,'issue':'private key'})
  if not rel.startswith('firmware-diffs/') and re.search(r'(?<![0-9a-fA-F])[0-9a-fA-F]{160,}(?![0-9a-fA-F])',text):errors.append({'file':rel,'issue':'large encoded hex payload outside a sparse diff'})
  if p.suffix=='.json':
   try:json.loads(text)
   except Exception as e:errors.append({'file':rel,'issue':'invalid JSON '+str(e)})
  if p.suffix=='.py':
   try:compile(text,str(p),'exec')
   except Exception as e:errors.append({'file':rel,'issue':'invalid Python '+str(e)})
  if p.suffix=='.md':
   for target in re.findall(r'(?<!!)\[[^\]]+\]\(([^)\n]+)\)',text)+re.findall(r'(?:src|href)="([^"]+)"',text):
    target=target.split('#',1)[0]
    if not target or '://' in target or target.startswith('<'):continue
    links+=1
    if not (p.parent/target).exists():errors.append({'file':rel,'issue':'missing local link '+target})
  if rel not in EXEMPT:rows.append({'path':rel,'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest()})
 manifest_path=ROOT/'verification/PACKAGE_MANIFEST.json'
 if manifest_path.exists():
  expected=json.loads(manifest_path.read_text(encoding='utf-8'))
  if expected!=rows:errors.append({'file':'verification/PACKAGE_MANIFEST.json','issue':'package file/hash manifest differs; regenerate only after review'})
 coverage=json.loads((ROOT/'verification/COVERAGE.json').read_text(encoding='utf-8'))
 if not coverage['translation_complete']:errors.append({'file':'verification/COVERAGE.json','issue':'translation incomplete'})
 if coverage.get('untranslated_or_unmapped_source_paragraphs_including_duplicate_occurrences',1)!=0:errors.append({'file':'verification/COVERAGE.json','issue':'unmapped source narrative remains'})
 translation=ROOT/'verification/TRANSLATION_CHECKS.json'
 if translation.exists():
  tr=json.loads(translation.read_text(encoding='utf-8'))
  if not tr['all_blocks_present'] or not tr['exact_placeholder_checks'] or tr['errors']:errors.append({'file':'verification/TRANSLATION_CHECKS.json','issue':'translation integrity check incomplete'})
 else:errors.append({'file':'verification/TRANSLATION_CHECKS.json','issue':'missing translation integrity check'})
 diffs=list((ROOT/'firmware-diffs').glob('*.diff.json'))
 if len(diffs)!=24:errors.append({'file':'firmware-diffs','issue':'expected24identified stock-relative variants'})
 for p in diffs:
  d=json.loads(p.read_text(encoding='utf-8'));rel=p.relative_to(ROOT).as_posix()
  if d.get('stock_image_sha256')!='477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89' or d.get('stock_image_bytes')!=96532:errors.append({'file':rel,'issue':'wrong stock reference'})
  try:
   changed=sum(len(bytes.fromhex(s['expected_stock_hex'])) for s in d['spans']);added=len(bytes.fromhex(d['appended_authored_difference_hex']))
   if d['unchanged_stock_bytes_included']!=0 or changed!=d['changed_original_bytes'] or changed>int(.08*96532) or added>32768 or d['result_image_bytes']!=96532+added:raise ValueError('non-sparse or inconsistent accounting')
   if any(any(a==b for a,b in zip(bytes.fromhex(s['expected_stock_hex']),bytes.fromhex(s['replacement_hex']))) for s in d['spans']):raise ValueError('unchanged stock positions in diff')
  except Exception as e:errors.append({'file':rel,'issue':'invalid sparse diff '+str(e)})
 result={'verified':not errors,'files':len(files),'manifested_files':len(rows),'bytes':sum(x['bytes'] for x in rows),'local_links_checked':links,'allowlisted_extensions':sorted(ALLOWED),'firmware_images':0,'decompilation_or_disassembly_files':0,'raw_vendor_assets':0,'hardware_actions':False,'coverage':coverage,'errors':errors}
 return result,rows
if __name__=='__main__':
 result,rows=verify();print(json.dumps(result,indent=2));sys.exit(0 if result['verified'] else 1)
