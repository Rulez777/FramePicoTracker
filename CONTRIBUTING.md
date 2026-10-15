# Continue the work

Thank you for taking an interest. The original author may not be available for support, so useful changes should stand on their own evidence.

Start with [QUICK_SETUP.md](QUICK_SETUP.md), [AGENTS.md](AGENTS.md) and [the current status](docs/PUBLIC_STATUS.md). Work in a separate development checkout. Do not build or fetch dependencies into the verified reference folder.

## A useful first contribution

Pick one task from [CONTRIBUTION_TASKS.md](docs/CONTRIBUTION_TASKS.md). State what you observed, what you changed, what input versions you used and how someone else can check the result. Keep failed attempts and uncertainty visible. Tests using generated data must say so.

Do not commit firmware images, decompilation, vendor SDK code, device calibration, recorded camera or radio payloads, credentials, personal device IDs, build outputs or virtual environments. Keep those privately and record only permitted provenance and hashes. Proposed PICO firmware changes must remain exact stock relative diffs.

Device work requires your own hardware, authorized access, current identity checks and an explicit bounded procedure with restoration. A historical successful run is not a reason to bypass identity, timing, covariance, IMU or controller guards. No automatic flashing, radio sweep or startup service belongs in a newcomer setup.

## How to describe results

Use Hardware for a measured real device result, Static for code or firmware inspection, Recorded/Offline for replay, and Inferred for a conclusion not yet tested. A compiled application, radio reply or registered tracker is not proof of a valid pose. Provide the specific evidence needed for your claim.

For code changes, add a focused regression when appropriate, document configuration changes and preserve the recovery path. Include dependency versions and build commands if you improve reproducibility. Do not overwrite historical summaries to make an earlier failure disappear.

## Credit and licensing

Your original contributions are submitted under the repository's GPL version 3 or later terms unless an explicit exception is agreed before inclusion. Preserve existing notices, add your own contribution to the record and identify modifications. No copyright assignment is requested. Do not submit code or data you lack permission to share.
