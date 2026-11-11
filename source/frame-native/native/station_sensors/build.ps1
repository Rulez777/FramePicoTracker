param([string]$Clang, [string]$Lld, [string]$OutDir="$PSScriptRoot/../../outputs/sen1")
$ErrorActionPreference='Stop'
if(!$Clang){$Clang=(Get-Command clang.exe -ErrorAction SilentlyContinue).Source}
if(!$Clang){$Clang=(Get-ChildItem 'C:/Program Files/Unity' -Recurse -Filter clang.exe -ErrorAction SilentlyContinue | Select-Object -First 1).FullName}
if(!$Clang){throw 'Supply -Clang with the ARM-capable clang executable'}
if(!$Lld){$Lld=Join-Path (Split-Path $Clang) 'ld.lld.exe'}
New-Item -ItemType Directory -Force $OutDir | Out-Null
$obj=Join-Path $OutDir 'sensors.o';$elf=Join-Path $OutDir 'sensors.elf'
& $Clang --version
& $Clang --target=arm-none-eabi -mcpu=cortex-m4 -mthumb -Oz -flto -fomit-frame-pointer -ffreestanding -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -c "$PSScriptRoot/roy_station_sensors.c" -o $obj
if($LASTEXITCODE){throw 'ARM compile failed'}
& $Lld --mllvm=-enable-machine-outliner=always --mllvm=-machine-outliner-reruns=2 --gc-sections -T "$PSScriptRoot/roy_ram_rx.ld" $obj -o $elf
if($LASTEXITCODE){throw 'ARM link failed (2044-byte reservation unchanged)'}
$bytes=[IO.File]::ReadAllBytes($elf)
$offset=[BitConverter]::ToUInt32($bytes,32);$stride=[BitConverter]::ToUInt16($bytes,46);$count=[BitConverter]::ToUInt16($bytes,48)
$textSize=0
for($i=0;$i -lt $count;$i++) {
 $section=$offset+$i*$stride
 if([BitConverter]::ToUInt32($bytes,$section+8) -band 4){$textSize += [BitConverter]::ToUInt32($bytes,$section+20)}
}
if(!$textSize -or $textSize -gt 2044){throw "Invalid executable size $textSize"}
".text=$textSize limit=2044 spare=$(2044-$textSize)"
Get-FileHash -LiteralPath $elf -Algorithm SHA256 | Format-List
