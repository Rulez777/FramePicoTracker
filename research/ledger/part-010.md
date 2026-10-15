# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00721
Source `D0222`, original line37, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

The reproducer queues command22 before block1. Forty long requests return only
command22 although the worker wrote the block exactly once. A short one-byte
S1=2 poll then returns both command22 and the data application ACK.
The old sender times out block1 with zero acknowledged blocks in this model.

## K00722
Source `D0222`, original line42, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

## Relation to the hardware failure: strong hypothesis, not readback

## K00723
Source `D0222`, original line44, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Attempt32 tag20 contained248 IMU24-byte records,245 IMU18-byte records and three
command22 records with23-byte body. Command22 had the same sequence1 at slots
305,435,477. This matches a retained reliable record and the modeled queue
blocking mechanism. It does **not** prove the real block was written or its ACK
was queued. Request reception, worker execution and staging contents were not
read back. The hardware result remains0/4056 acknowledged, no finish.
Do not claim that this audit established the unique hardware root cause.

## K00724
Source `D0222`, original line52, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

The earlier pending-worker/unmarked-start-ACK race and native CRC8 collision
remain unresolved. Button reboot is not verified fresh application startup;
bootloader, enlarged-image acceptance, recovery and installed fork identity are
still unknown. A software fix does not close those hardware unknowns.

## K00725
Source `D0222`, original line57, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

## Sender correction and integration

## K00726
Source `D0222`, original line59, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Service27 now selects `OtaSession(response_poll=True)`. After a valid response
from the **current published data-request tag** without its application ACK,
the sender publishes a short S1=2 poll. This restores the54-byte response
budget without retiring unread notifications. A0.25s retry restores exactly
the same command sequence and block body, under a fresh content generation.
Matching application ACKs on earlier published tags within this operation remain
eligible; tags from previous operations are discarded. A stale non-ACK cannot
cancel a new request, including when `receive()` crosses the retry deadline.

## K00727
Source `D0222`, original line68, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Every mailbox content change uses a new even generation. The zero-retry budget
is now `3*blocks+5` for this policy; retries consume additional finite tag space.
Exhaustion aborts without wrap/reuse or automatic resume. Default non-service27
sender policy is unchanged. S1=3 drain begins only after the application ACK.
Per-data/per-drain deadline remains10 seconds.

## K00728
Source `D0222`, original line74, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

The adapter logs changed host publications to `.tx.jsonl`, including sequence,
generation, S1 and exact OTA packet. This is host publication evidence, not proof
of actual RF TX. The live pause creates no sender and no such publication log.

## K00729
Source `D0222`, original line78, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

The overall service27 transfer deadline is bounded at7200 seconds; other profiles
retain1800. This addresses a separate duration concern, not block1 retry failure:
the ordinary live tail contained24 empty replies in20.04s, with mean observed
inter-empty gap0.849s and maximum2.104s. Multiplying the mean gap by4056 gives
57.4min as a sensitivity estimate. This is **not** a measured OTA duration or an
estimate of forward waiting time. OTA mode/ACK conditioning may differ. The
previous optimistic8117-poll estimate excluded worker and drain latency. A real
transfer may exceed30min; its speed must be measured after separately authorized
start. Two hours is a finite allowance, not a guarantee of completion.

## K00730
Source `D0222`, original line88, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

## Validation and review

## K00731
Source `D0222`, original line90, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Final focused suite: **70 passed**, `outputs/ota33/final-tests.xml`.
Earlier extended suite:25 passed,1 intentionally deselected (the previous slow
full runtime/file-write campaign); its scope is separately recorded.
Checks cover native head-of-queue blocking, old-policy timeout, exact full fork,
native prioritized IMU, lost request, retry-edge delayed reply, tag exhaustion,
service runtime selection/publication log, live pause and authorization gates.

## K00732
Source `D0222`, original line97, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

`native-response-audit.json`: full97320-byte revision05 fork,4056 acknowledged
blocks including sentinel, one erase,4056 writes, exact reconstructed staging
SHA256 `395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f`.
16231 modeled exchanges, final generation24346,4056 short polls, no retries.
RF timing, radio device registers, SPI, IRQ and fixed-block allocation remain
modeled/mocked. This is neither hardware throughput nor boot/recovery evidence.

## K00733
Source `D0222`, original line104, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7 medium broad review timed out after630s; saved separately. Narrow review
returned and agreed on the modeled queue defect, scoped the hardware inference,
and found the stale non-ACK/retry-edge issue in an earlier draft. The current-tag
condition and both retry-edge tests resolve it. It also confirmed that baseline
tag reservation excludes retries; comments and this report explicitly say so.
Review: `outputs/ota33/grok-review.json`. No fast model was used.

## K00734
Source `D0222`, original line111, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

## Live checks and actual stopping point

## K00735
Source `D0222`, original line113, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Installed sender/runtime imports and hashes were verified on Frame. The service27
ELF remains unchanged:SHA256 `65d217e5e47e8028a6d58fe46c17923fcd20877dff27ffed64bc4dbf71f64a44`.
Only host Python was changed; original installed files were backed up.
First wrapper failed its metadata guard before loader/debugger/RF: the generic
offline package omitted target_serial. Its failure was preserved. A separate
live-job added the exact serial and proceeded with the mandatory CLI pause.

## K00736
Source `D0222`, original line120, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

The bounded65-second live check confirmed serial[TRACKER_SERIAL],
chip LE3eed1979,FW189, controllers-off guards and actual state
`paused_before_ota_start`, `ota_start_sent=false`. No OtaSession was constructed,
no start/erase/data/finish was issued and no tracker activation followed.

## K00737
Source `D0222`, original line125, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Whole run:2863 retained unique replies /3235 actual TX-END = **88.5008%**;
2874 hardware CRC RX /3235 TX-END =88.8408%, nine ring drops.
The saved last1000 records span1007 inclusive scheduled slots: **99.3049%**
retained/scheduled in that tail. Tail TX-END was not independently snapshotted;
do not relabel this denominator as actual TX-END. Final saved header deltas are
179 CRC RX /179 scheduled slots. Do not replace the whole-run result with a
selected stable window or conflate ordinary replies with OTA block ACKs.
Analysis: `outputs/ota33/live-analysis.json`.

## K00738
Source `D0222`, original line134, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Roy session, SRAM, MPU and trace restoration all verified. Normal custom radio
and paused unit are inactive; no loader/OpenOCD remains. Tracker application
responded during the check; its present power/sleep state is not asserted.
After capture, only the host overall-timeout allowance was changed to7200s;
final installed hashes/imports and pause tests were reverified without new RF.
`capture/final-installed.json` distinguishes this from the earlier capture SHA.

## K00739
Source `D0222`, original line141, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

## Next action remains paused

## K00740
Source `D0222`, original line143, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

The latest fork is still dual-mode revision05; its binary was not modified or
installed. `outputs/ota33/package/` is a new reviewed offline package, not an OTA
authorization. The paused live-job/result and failed attempt32 cannot be replayed.
After a separate human start instruction, create a fresh job/result, recheck the
live exact identity/controllers/config/imports, then perform at most one reviewed
attempt. The next first destructive command is OTA start, which may erase SPI
staging. No automatic retry, reset, finish, activation or hidden resume is prepared.
Current stopping point is immediately before this procedure.

## K00741
Source `D0223`, original line1, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

# Iteration38: tag-zero reliable ACK retirement defect

## K00742
Source `D0223`, original line3, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User explicitly requests fixing and proceeding. Exact target and
revision05 image unchanged; unavailable recovery risk remains accepted.

## K00743
Source `D0223`, original line6, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

## Hardware37 outcome, preserved

## K00744
Source `D0223`, original line8, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

After waking the target, exact serial/FW189 verified live. New capture-ACK36
attempt accepted start and11 indexed blocks, then timed out10s on block12.
76 retry intervals,50 response polls; no finish, no verified fork installation.
Cleanup session/SRAM/MPU/trace all true, no loader/OpenOCD remains.
Evidence outputs/ota37/attempt and response-analysis.json. Full event history
and TX publication log preserved. Raw report retains only last1000 records.

## K00745
Source `D0223`, original line15, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

Actual block12 success ACK2502000c01 appears at tx_generation0, reliable
sequence13, among last1000 raw records. Later block12 request tags have no
accepted indexed ACK. Compiled36 sends a tag0 fallback empty packet when
mailbox seqlock is odd/changes. Capture ACK token then acknowledges any valid
retained latest reply, including tag0. OTA host rejects tag0 (never published
for that operation). Thus a retained but host-ineligible ACK can be retired.
This is a concrete station/host contract defect; scheduler-admission37 remains
an independent offline defect, not the established cause of this block12 stall.

## K00746
Source `D0223`, original line24, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

## Narrow candidate38

## K00747
Source `D0223`, original line26, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

Clone service36 to pc/radio/ram/roy_ram_station_service38.c; preserve36.
Only arm capture ACK token when D1648 transmission tag is nonzero, in addition
to hardware CRC, checksum, working stage and full retention. Tag0 replies
remain diagnostic records and cannot retire their reliable notification list.
Fresh tagged short poll can deliver the same indexed ACK later. Never accept
tag0 at host or broaden stale-operation tag authorization.

## K00748
Source `D0223`, original line33, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

ELF outputs/ota38/build/roy_station.elf,2036/2045B,
SHA25644f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c,
code SHAb14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a.
Build manifest records source/linker/compiler. Narrow service38 profile uses
exact same image/target/radio constraints; installed Python backed up under
Frame ota38/before-install. Native test proves tag0 ACK remains queued, later
tagged delivery permits one-shot ACK. Full-image campaign pending at this point.
65 focused host/runtime/guards tests pass. Grok37 review timed out630s; no
review conclusions available. New physical38 attempt has not yet started.

## K00749
Source `D0223`, original line43, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

Full-image compiled/native campaign completed:2 tests pass, exact4056 blocks
with modeled10% losses/overflow. Physical38 launched with hash/live identity
guards; first13 blocks ACKed, beyond37 block12 stall. Outcome still pending.
No boot or finish claim. Model does not include real RF/peripheral timing.

## K00750
Source `D0223`, original line48, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

Physical38 progressing:136/4056 indexed blocks confirmed, no error at latest
check. Active session is intentionally kept running, not interrupted/restarted.
Missing full finish/readback/boot proof; final hardware outcome not established.

## K00751
Source `D0223`, original line52, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

## Actual physical38 outcome

## K00752
Source `D0223`, original line54, SHA256 `d32c9cd256965ae5e8fd9af0312181240fe42f803412ac109451d40fc39ab8a7`. Status: historical/unadjudicated narrative; English source prose.

549/4056 blocks ACKed (13.5355%), then drain_ack before block550 timed out10s.
2302 retry intervals; no finish, no verified installation. Cleanup all true.
Evidence outputs/ota38/attempt. Last1000 RX records contain telemetry and
last ACK2502022501 (index549), no next block submitted. Native scheduler defect
is not implicated by this drain state. Physical RF remains responsive.

## K00753
Source `D0224`, original line1, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

# OTA transfer audit29: full fork under faults, installation still blocked

## K00754
Source `D0224`, original line3, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. Offline development only. No SSH, SWD, RF, device connection, reset,
OTA start, flash write, whitelist change or hardware-gate change was performed.

## K00755
Source `D0224`, original line6, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

## Current historical baseline

## K00756
Source `D0224`, original line8, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

The current binary remains iteration05,97320 bytes, SHA256
`395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f`.
This does not make iteration05 the latest project research. Reports07/08 change
host mode-refresh behavior;09/10/11 update the SWD read-only operator plan.
The latest plan is revision11. All were read during this audit. Ten relevant
historical documents match the snapshot manifest and their live copies, recorded
in `outputs/ota-transfer29/provenance.json`.

## K00757
Source `D0224`, original line16, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

Revision11 is documentation, not a new firmware build or a completed dump. It
does not authorize tracker attachment; target/probe/tool/pinout remain unassigned.
DHCSR/DEMCR writes are excluded by default; asserted DP power requests must be
cleared and completion recorded. These corrections do not close bootloader,
recovery or clean-start OTA blockers. See
SWD plan revision11 (original reference: `FW189_SWD_READONLY_PLAN_2026-10-08.md`) and
iteration11 (original reference: `FW189_ITERATION_11_SWD_DEBUG_POWER_2026-10-08.md`).

## K00758
Source `D0224`, original line24, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

## Complete fork fault campaign

## K00759
Source `D0224`, original line26, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

`pc/tests/ota_fault_harness.py` drives the current OtaSession against actual stock
FW189 ARM queue/dispatcher/dedup/application OTA instructions. The97320-byte fork
is the transfer payload; the receiver remains stock. SPI I/O, IRQ helpers and
fixed-block allocation are mocked. This is not bootloader execution, RF hardware
or a cycle-accurate throughput simulation.

## K00760
Source `D0224`, original line32, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

The campaign injects independent TX/RX omissions,18 consecutive omitted TX polls
every503 frames,1% CRC-invalid replies, malformed application checksums,
one-frame delayed replies, duplicate replies and previous-operation tags.
The0.018% omission case is a hypothetical model near the observed reply-loss
fraction; that observation does not prove equal independent loss probabilities
in both directions. One deterministic random seed29 is used, not exhaustive
state-space coverage.

## K00761
Source `D0224`, original line40, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

| Omission probability per direction | Poll frames | Modeled seconds | Sender retries | Staging result |
|---|---:|---:|---:|---|
|0.018%|8594|176.177|18|Exact fork SHA256|
|1%|8729|178.9445|18|Exact fork SHA256|
|10%|10638|218.079|22|Exact fork SHA256|

## K00762
Source `D0224`, original line46, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

Each completed run performs one native erase and4056 writes, including the empty
sentinel after4055 full24-byte blocks. Native record deduplication prevents
repeated transmissions from repeating erase/write operations. The reconstructed
SPI page layout matches the full fork byte for byte. Evidence is
`outputs/ota-transfer29/fault-campaign.json`; repeatable offline runner is
`outputs/ota-transfer29/run.py`.

## K00763
Source `D0224`, original line53, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

A separate corruption test changes staging before finish; native finish returns
a negative application ACK and the sender fails. An initial harness version
changed staging after executing the finish worker, so it tested post-validation
corruption rather than finish rejection. The harness was corrected to alter
staging before the worker; all final tests and artifacts use that ordering.

## K00764
Source `D0224`, original line59, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

## Integrity and ACK limits

## K00765
Source `D0224`, original line61, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

Another native test deliberately constructs two different24-byte images sharing
CRC8/MAXIM, changes staging to the second image, and observes successful native
finish. Stock whole-image CRC8 is not SHA256 identity proof and cannot guarantee
rejection of every corruption. Hardware packet CRC, application checksum and
whole-image CRC8 are separate checks; the exact reconstructed SHA256 comparison
in this campaign is available only in the offline model.

## K00766
Source `D0224`, original line68, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

Previous-generation ACKs, duplicates and incorrect block indices are rejected by
the sender. The existing native pending-worker race test also remains passing:
it demonstrates the unresolved failure, not its correction. A stock old worker
can finish after the visible queue becomes empty and emit an unmarked start ACK
under a fresh RF tag. New host generation or an empty-reply barrier does not
prove that this ACK belongs to the new image/session. The gate remains closed.

## K00767
Source `D0224`, original line75, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

Future fork-to-fork OTA should include image identity, session nonce and indexed
application acknowledgments plus strong whole-image integrity. Such an extension
cannot retroactively change the stock receiver for the first installation.

## K00768
Source `D0224`, original line79, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

## Sender correction

## K00769
Source `D0224`, original line81, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

OtaSession previously accepted NaN/infinite deadline values, which could defeat
timeout comparisons. It now requires finite positive numeric limits, rejects
booleans/non-numeric values, and requires an integer command sequence. This
keeps the native wire format unchanged. Subsequent reviewer fixes below change
retry tagging and conservative drain acceptance. No host changes were deployed to Frame in this audit.

## K00770
Source `D0224`, original line87, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

## Concrete offline preflight

## K00771
Source `D0224`, original line89, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

`pc/radio/tracker_ota_transfer_audit.py` verifies the fork manifest and reverse to
the frozen stock image, inventories block/sentinel count and combines them with
the saved iteration28 poll rate. Output:
`outputs/ota-transfer29/preflight.json`. It is explicitly not an executable job.

## K00772
Source `D0224`, original line94, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

At the saved48.070447Hz poll rate,8117 minimum successful poll exchanges require
an optimistic168.856 seconds. This excludes erase/program/worker latency,
startup, scheduling delay and retries. It is not a measured firmware-transfer
time. Native emulation timings above assume a20.5ms poll period and immediate
worker execution and likewise are not hardware predictions.

## K00773
Source `D0224`, original line100, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

The preflight records six unresolved requirements:

## K00774
Source `D0224`, original line102, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

1. Verified bootloader acceptance of the enlarged image.
2. Demonstrated recovery after a nonbooting application.
3. Clean startup/no prior pending OTA worker.
4. Hardware job support for the expanded dual-mode image: current load_job accepts
   only frozen stock or the older same-size minimal patch.
5. Integrating the measured service27 cadence: the existing exclusive adapter
   still requires cycle7/N9/6000us, not the measured cycle1/N0/4727us profile.
6. Actual block/application-ACK throughput on hardware; ordinary poll success
   is not that measurement.

## K00775
Source `D0224`, original line112, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

These are not resolved by editing approval JSON or loosening the whitelist.
Next hardware-evidence step remains the authorized read-only SWD plan, after
physical target/access/tool details and exact behavior have been established.

## K00776
Source `D0224`, original line116, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

## Validation and provenance

## K00777
Source `D0224`, original line118, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

79 tests pass, saved JUnit `outputs/ota-transfer29/tests-final.xml`: full fork fault
campaign, native corruption/CRC8 collision, sender timeout validation, OTA
runtime/preflight, stock authorization, native queue and readiness regressions.
Tests also confirm that the hardware gate rejects before hardware ownership.

## K00778
Source `D0224`, original line123, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

Firmware image and historical reports are unchanged. New artifacts, source
hashes and evidence are indexed in `outputs/ota-transfer29/artifact-manifest.json`.
Grok4.7 medium completed its independent read-only review, saved as
`outputs/ota-transfer29/grok-review.json`. Two findings were reproduced and fixed:

## K00779
Source `D0224`, original line128, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

- With initial generation49300, exactly8117 tags remain. A forced lost start
  reply plus a retry exhausted tags after one erase and4056 writes, leaving the
  sender failed. Reproduced evidence: `tag-boundary-before.json`. Retries now keep
  the unchanged operation's tag; only a new operation consumes a tag. The same
  full-fork scenario now completes with exact staging at generation65534, one
  erase and4056 writes: `tag-boundary-after.json`. No automatic tag wrap is allowed.
  The MCU continuously transmits the unchanged mailbox; republishing an identical
  mailbox is unnecessary. This does not add a session marker to stock ACKs.
- ACK-drain now requires the exact empty payload, as pre-start already did.
  Native queue execution with a6-byte reply budget shows one non-OTA reliable
  record while an OTA ACK remains hidden behind it. Such a reply no longer moves
  the sender to the next operation. A telemetry-only regression is also included.
  Continuous nonempty traffic may now time out conservatively rather than falsely
  claim an empty queue; the existing deadline remains active.

## K00780
Source `D0224`, original line143, SHA256 `0a5fd146bb6c1864f68fb3ef0b602c540dfd6d3bdbded72fca8b5af3730d7d36`. Status: historical/unadjudicated narrative; English source prose.

The full fault campaign was rerun after both changes and remains successful;
`fault-campaign.json` preserves earlier results and `fault-campaign-after.json`
contains final results. The stock pending-worker negative control remains
unchanged and reproduced. No hardware/whitelist/gate changes follow this review.

## K00781
Source `D0225`, original line1, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

# Phase100 run4: bounded transition grace

## K00782
Source `D0225`, original line3, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Goal: 98–100% tracker delivery while both Frame controllers remain healthy. This goal is not yet demonstrated.

## K00783
Source `D0225`, original line5, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

The user authorized variant A. The isolated Phase100 candidate retains the Phase99 module and continuous session. Controller readiness is shortened to 2 seconds. Phase measurement lasts 8 seconds with controller protection in AWAIT; HMD and radio liveness checks remain active. Strict protection (two controllers, 150 ms freeze limit) must succeed after 3 seconds of dense replies, before command 19 enables IMU. A hard 12-second deadline from S2 entry aborts an incomplete transition. Fresh Frame mode is required before S1.

## K00784
Source `D0225`, original line7, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Offline validation: 25 tests passed. Grok 4.7 medium independently found no blocking defect in the transition guard. Its review is not hardware reception or OTA approval.

## K00785
Source `D0225`, original line9, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Staged source SHA256: `c3fa5461d0cc1d1b8815fa7a7bf57174bceb21b4538dfeed09543c0149a44422`.
Unchanged ELF SHA256: `a9b882d9d84bb6f42f0134f61d87516b9368acd8b750cae859258f219817bad1`.

## K00786
Source `D0225`, original line12, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Evidence: `outputs/phase100/offline-tests.xml`, isolated release, preparation script, and run4 attempt records. Phase99 historical files are preserved. No OTA or persistent firmware change is involved.

## K00787
Source `D0225`, original line14, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Important metric limitation: this module does not count completed TX events. Replies divided by scheduled slots includes blocked slots and cannot establish actual transmission delivery efficiency. A stable connection with zero IMU is not the requested streaming result.

## K00788
Source `D0225`, original line16, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Correction inherited from Phase91–99: the approximately 85 microsecond late-response group corresponds to the longer host mode query (21 extra bytes at 2 Mbit/s), not proven tracker latency. Alternating controller loss with a roughly 10–11 second period suggests scheduling interference; a beat mechanism remains a hypothesis.

## K00789
Source `D0225`, original line18, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Run4 was launched after the user confirmed controllers OFF. The target identified as [TRACKER_SERIAL], firmware 189, and fresh Frame mode was confirmed. The user was then asked to turn both controllers ON. Results and cleanup will be appended after the bounded run.

## K00790
Source `D0225`, original line20, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

## Hardware result

## K00791
Source `D0225`, original line22, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Run4 elapsed 49.188458 seconds. S1 began at 2.547281 seconds; S2 began at 41.155725 seconds. S2 completed approximately 8 seconds of measurement, then aborted because the folded histogram contained no admissible quiet window. S3 and S4 were never reached; command 19 was not queued.

## K00792
Source `D0225`, original line24, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

| Stage | Scheduled peer polls | Replies | IMU | Ring drops |
|---|---:|---:|---:|---:|
| S0 | 50 | 44 | 0 | 0 |
| S1 | 4810 | 728 | 0 | 0 |
| S2 | 192 | 190 | 0 | 0 |

## K00793
Source `D0225`, original line30, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

S2 replies/scheduled polls = 98.9583%, but this is a short keepalive phase, not the requested IMU result and not a completed-TX delivery measurement. Final live count: 962 replies / 5352 scheduled slots (17.9746%); the differing aggregate denominators include non-peer/bootstrap scheduling. The final S2 controller metric window recorded controller 2 invalid in 24/99 samples, maximum freeze 232.585757 ms. Transition AWAIT explicitly permits these temporary controller failures. No evidence of healthy simultaneous streaming was obtained.

## K00794
Source `D0225`, original line32, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

All four cleanup checks passed: session closed, RAM restored, MPU restored, trace controls restored. The wrapper exited with an assertion because the experiment aborted; this does not indicate cleanup failure.

## K00795
Source `D0225`, original line34, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

## Sampling limitation found in the raw log

## K00796
Source `D0225`, original line36, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

`outputs/phase100/analyze_sample_spans.py` recovers candidate four-word RTC/state/BASE/RTC results from `run4/attempt/openocd.log`. It selected 1727 rows, excluded 12 own-radio busy rows, and reconstructed essentially the reported busy histogram. Minor differences mean this shape-based extraction is diagnostic, not an exact substitute for explicit staged raw records.

## K00797
Source `D0225`, original line38, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Measured duration between the two RTC reads: minimum 1312.256 us, median 1525.879 us, p90 2105.713 us, p99 2838.135 us, maximum 3662.109 us. The current sampler assigns the STATE read to the midpoint of this entire bracket, and the later BASE read is not simultaneous with STATE. These spans exceed the desired 540–600 us quiet interval. The absence of an admissible folded interval therefore does not establish absence of a physical BLE pause. Timing misassignment and STATE/BASE mismatch are plausible measurement errors; their exact contribution is unknown.

## K00798
Source `D0225`, original line40, SHA256 `75663867fd0629e2bddb0f98373c7aa8d2846da9212cb19c1296fa3a51db763c`. Status: historical/unadjudicated narrative; English source prose.

Next implementation should timestamp radio observations on the Nordic itself, preserve raw rows and capture uncertainty, and compare a stock-controller baseline against live coexistence. Do not simply relax the 5% busy threshold or reuse this histogram as a precise phase target. Keep the finite grace and strict admission before IMU.

## K00799
Source `D0226`, original line1, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

# Phase109: bounded transition and observer candidate

## K00800
Source `D0226`, original line3, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

## Evidence before this change
