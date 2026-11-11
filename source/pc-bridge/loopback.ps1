param([string]$Build = "$PSScriptRoot/build-msvc")
$ErrorActionPreference = 'Stop'
$Build = (Resolve-Path -LiteralPath $Build).Path
$out = Join-Path $Build 'loopback-receiver.txt'
$err = Join-Path $Build 'loopback-receiver.err.txt'
$receiver = Start-Process -FilePath (Join-Path $Build 'pc_bridge_receive.exe') -ArgumentList '--bind','127.0.0.1:39691','--seconds','4','--expect-test' -WindowStyle Hidden -PassThru -RedirectStandardOutput $out -RedirectStandardError $err
$null = $receiver.Handle
Start-Sleep -Milliseconds 250
& (Join-Path $Build 'pico_pc_bridge_send.exe') --test --target 127.0.0.1:39691 --frames 120
$senderCode = $LASTEXITCODE
$receiver.WaitForExit()
$receiver.Refresh()
Get-Content -LiteralPath $out
Get-Content -LiteralPath $err
if ($senderCode -ne 0 -or $receiver.ExitCode -ne 0) { throw "Loopback failed: sender=$senderCode receiver=$($receiver.ExitCode)" }
