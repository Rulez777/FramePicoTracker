"""Search local handoff metadata without loading private assets or using the network."""
from pathlib import Path
import argparse,json
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('catalog',choices=['documents','knowledge','assets','source']);ap.add_argument('--contains',default='');ap.add_argument('--phase',type=int);ap.add_argument('--limit',type=int,default=30)
 a=ap.parse_args();names={'documents':'RESEARCH_DOCUMENTS.json','knowledge':'KNOWLEDGE_RECORDS.json','assets':'ASSET_REGISTER.json','source':'SOURCE_CODE_INDEX.json'}
 rows=json.loads((ROOT/'catalog'/names[a.catalog]).read_text(encoding='utf-8'))
 if a.contains:rows=[r for r in rows if a.contains.lower() in json.dumps(r).lower()]
 if a.phase is not None:rows=[r for r in rows if a.phase in r.get('phase_ids',[]) or ('phase'+str(a.phase)) in r.get('relative_source_path','').lower()]
 shown=rows if a.limit==0 else rows[:max(0,a.limit)]
 print(json.dumps({'matching_records':len(rows),'shown_records':len(shown),'truncated':len(shown)!=len(rows),'records':shown},indent=2))
if __name__=='__main__':main()
