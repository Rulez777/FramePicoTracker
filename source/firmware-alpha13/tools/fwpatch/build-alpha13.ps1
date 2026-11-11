param([string]$Zig='<PROJECT>')
$ErrorActionPreference='Stop'
$env:ZIG_GLOBAL_CACHE_DIR="$pwd/outputs/zig-global"
$env:ZIG_LOCAL_CACHE_DIR="$pwd/outputs/zig-local"
function Run([string[]]$ArgList) {
  & $Zig @ArgList
  if($LASTEXITCODE -ne 0){throw "Compiler failed: $ArgList"}
}
Run @('c++','-std=c++17','-O2','-Wall','-Wextra','-Werror','tools/fwpatch/main.cpp','-o','outputs/fwpatch.exe')
Run @('cc','-target','thumb-freestanding-eabi','-mcpu=cortex_m4','-DALPHA12','-DALPHA13','-c','tools/fwpatch/frame-alpha1.S','-o','outputs/frame-alpha13.o')
Run @('cc','-target','thumb-freestanding-eabi','-mcpu=cortex_m4','-O1','-falign-functions=2','-ffreestanding','-fno-builtin','-Wall','-Wextra','-Werror','-DALPHA12','-DALPHA13','-c','tools/fwpatch/alpha1.c','-o','outputs/alpha13-base.o')
Run @('cc','-target','thumb-freestanding-eabi','-mcpu=cortex_m4','-O1','-falign-functions=2','-ffreestanding','-fno-builtin','-Wall','-Wextra','-Werror','-DALPHA13','-c','tools/fwpatch/alpha12.c','-o','outputs/alpha13.o')
Run @('cc','-target','thumb-freestanding-eabi','-mcpu=cortex_m4','-O1','-falign-functions=2','-ffreestanding','-fno-builtin','-Wall','-Wextra','-Werror','-c','tools/fwpatch/alpha13.c','-o','outputs/alpha13-measure.o')
Run @('build-exe','-target','thumb-freestanding-eabi','-mcpu=cortex_m4','-fno-entry','-T','tools/fwpatch/frame-release.ld','outputs/frame-alpha13.o','outputs/alpha13-base.o','outputs/alpha13.o','outputs/alpha13-measure.o','-femit-bin=outputs/frame-alpha13.elf')
& outputs/fwpatch.exe release outputs/frame-alpha13.elf outputs/frame-alpha13 | Tee-Object outputs/accounting-alpha13-clang.txt
if($LASTEXITCODE -ne 0){throw 'Image accounting failed'}
& outputs/fwpatch.exe outputs/diagnostic-alpha13 | Tee-Object outputs/diagnostic-alpha13-clang.txt
if($LASTEXITCODE -ne 0){throw 'Diagnostic reproduction failed'}
& outputs/fwpatch.exe check-elf outputs/reassembled.elf | Tee-Object outputs/assembly-alpha13-clang.txt
if($LASTEXITCODE -ne 0){throw 'Original assembly comparison failed'}
Run @('c++','-std=c++17','-O2','-Wall','-Wextra','-Werror','tools/fwpatch/verify.cpp','-o','outputs/fwverify.exe')
& outputs/fwverify.exe | Tee-Object outputs/verification-previous-alpha13-clang.txt
if($LASTEXITCODE -ne 0){throw 'Previous verification failed'}
Run @('c++','-std=c++17','-O2','-Wall','-Wextra','-Werror','tools/fwpatch/alpha_verify.cpp','-o','outputs/alpha_verify.exe')
& outputs/alpha_verify.exe outputs/frame-alpha13/image.bin outputs/frame-alpha13.elf | Tee-Object outputs/verification-alpha13-clang.txt
if($LASTEXITCODE -ne 0){throw 'Alpha 1 verification failed'}
