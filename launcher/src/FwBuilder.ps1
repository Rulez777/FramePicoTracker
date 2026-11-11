#Requires -Version 5.1
<#
.SYNOPSIS
Сборка образа Roy из открытого пакета FW189 или локального файла.
.DESCRIPTION
Проверяет SHA256 входа, копирует байты среза из манифеста, накладывает alpha13-patch.json и проверяет SHA256 выхода. Без параметров открывает форму Windows Forms. Устройство не прошивается.
.PARAMETER Manifest
Локальный манифест. По умолчанию manifest.json рядом с Build-FwImage.cmd.
.PARAMETER Patch
Файл alpha13-patch.json. По умолчанию поле patchFile манифеста.
.PARAMETER InputPath
Локальный zip пакета или сам образ .img.
.PARAMETER Output
Куда записать собранный образ.
.PARAMETER Download
Скачать sourceUrl из манифеста.
.PARAMETER Gui
Открыть форму.
.PARAMETER Explain
Печать пояснения автоматической подготовки Roy и выход.
.PARAMETER GuiCheck
Собрать форму, проверить пояснение Roy и закрыть её без показа.
.EXAMPLE
Build-FwImage.cmd
.EXAMPLE
Build-FwImage.cmd -Download -Output C:\Temp\roy.bin
.EXAMPLE
Build-FwImage.cmd -Input D:\fw\HALO_FW_UPGRADE_189.img -Output D:\fw\roy.bin
#>
[CmdletBinding()]
param(
    [string]$Manifest = '',
    [string]$Patch = '',
    [string]$InputPath = '',
    [string]$Output = '',
    [switch]$Download,
    [switch]$Gui,
    [switch]$Explain,
    [switch]$GuiCheck
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

try {
    $utf8 = New-Object System.Text.UTF8Encoding $false
    [Console]::OutputEncoding = $utf8
    $OutputEncoding = $utf8
}
catch {
}

$Root = Split-Path -Parent $PSScriptRoot
$ModulePath = Join-Path $PSScriptRoot 'FwImage.psm1'
Import-Module $ModulePath -Force

if ([string]::IsNullOrWhiteSpace($Manifest)) {
    $Manifest = Join-Path $Root 'manifest.json'
}

$bundledManifest = Join-Path $Root 'manifest.json'
$otaDirectory = Join-Path (Split-Path $Root) 'frame/pico-ram-work/native-port-r3-20261101/outputs'
$defaultOtaOutput = Join-Path $otaDirectory 'frame-alpha13.bin'

function Write-RoyExplanation {
    $document = Read-FwManifest -Path $Manifest
    Write-Output (Get-RoyPreparationText -Manifest $document)
}

function New-FwBuilderForm {
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing
    [System.Windows.Forms.Application]::EnableVisualStyles()
    $form = New-Object System.Windows.Forms.Form
    $form.Text = 'Firmware Image Builder'
    $form.StartPosition = 'CenterScreen'
    $form.ClientSize = [Drawing.Size]::new(560,290)
    $form.FormBorderStyle = 'FixedDialog'
    $form.MaximizeBox = $false
    $form.Font = [Drawing.Font]::new('Segoe UI',10)
    $form.BackColor = [Drawing.Color]::White
    $state = @{ Manifest = $Manifest; Busy = $false }

    function Add-Control($type,$name,$caption,$x,$y,$w,$h) {
        $control = New-Object "System.Windows.Forms.$type"
        $control.Name = $name
        $control.Text = $caption
        $control.Bounds = [Drawing.Rectangle]::new($x,$y,$w,$h)
        $form.Controls.Add($control)
        return $control
    }
    $title = Add-Control Label Title 'Firmware Image Builder' 24 20 512 36
    $title.Font = [Drawing.Font]::new('Segoe UI',19,[Drawing.FontStyle]::Bold)
    $profile = Add-Control Label Profile '' 24 72 365 32
    $choose = Add-Control Button ChooseProfile 'Choose manifest...' 394 67 142 34
    $download = Add-Control Button Download 'Download and build...' 24 123 250 42
    $local = Add-Control Button Local 'Build from local file...' 286 123 250 42
    $status = Add-Control Label Status 'Choose a source to build a verified firmware image.' 24 188 512 75
    $download.BackColor = [Drawing.Color]::FromArgb(31,89,194)
    $download.ForeColor = [Drawing.Color]::White
    $download.FlatStyle = 'Flat'
    $form.AcceptButton = $download
    $profile.Text = 'Manifest: ' + (Read-FwManifest -Path $state.Manifest).id

    $choose.Add_Click({
        $dialog = New-Object System.Windows.Forms.OpenFileDialog
        $dialog.Filter = 'Firmware manifest (*.json)|*.json'
        try {
            if ($dialog.ShowDialog($form) -ne 'OK') { return }
            $document = Read-FwManifest -Path $dialog.FileName
            $state.Manifest = $dialog.FileName
            $profile.Text = 'Manifest: ' + $document.id
            $status.Text = 'Manifest loaded. Choose a source to continue.'
        } catch {
            $status.Text = 'Could not load the manifest. ' + $_.Exception.Message
        } finally { $dialog.Dispose() }
    }.GetNewClosure())

    $buildImage = {
        param($sender,$eventArgs)
        if ($state.Busy) { return }
        $arguments = @{ ManifestPath = $state.Manifest }
        if ($sender.Name -eq 'Local') {
            $inputDialog = New-Object System.Windows.Forms.OpenFileDialog
            $inputDialog.Filter = 'Firmware files (*.bin;*.img;*.zip)|*.bin;*.img;*.zip|All files (*.*)|*.*'
            try {
                if ($inputDialog.ShowDialog($form) -ne 'OK') { return }
                $arguments.InputPath = $inputDialog.FileName
            } finally { $inputDialog.Dispose() }
        } else {
            $arguments.Download = $true
        }
        $save = New-Object System.Windows.Forms.SaveFileDialog
        $save.Filter = 'Firmware image (*.bin)|*.bin|All files (*.*)|*.*'
        $save.FileName = if ($state.Manifest -eq $bundledManifest) { 'frame-alpha13.bin' } else { 'firmware.bin' }
        if ($state.Manifest -eq $bundledManifest -and (Test-Path -LiteralPath $otaDirectory -PathType Container)) {
            $save.InitialDirectory = $otaDirectory
        }
        try {
            if ($save.ShowDialog($form) -ne 'OK') { return }
            $arguments.OutputPath = $save.FileName
        } finally { $save.Dispose() }
        $state.Busy = $true
        $download.Enabled = $local.Enabled = $choose.Enabled = $false
        $form.Cursor = [System.Windows.Forms.Cursors]::WaitCursor
        $status.Text = 'Building and verifying the firmware image...'
        $form.Refresh()
        try {
            $result = Invoke-RoyBuild @arguments
            $status.Text = "Verified image saved: $($result.OutputPath)"
        } catch {
            $status.Text = 'Build failed. ' + $_.Exception.Message
        } finally {
            $state.Busy = $false
            $download.Enabled = $local.Enabled = $choose.Enabled = $true
            $form.Cursor = [System.Windows.Forms.Cursors]::Default
        }
    }.GetNewClosure()
    $download.Add_Click($buildImage)
    $local.Add_Click($buildImage)
    return $form
}
if ($Explain) {
    Write-RoyExplanation
    exit 0
}

if ($GuiCheck) {
    $form = New-FwBuilderForm
    try {
        Write-Output 'GUI_OK'
    }
    finally {
        $form.Dispose()
    }
    exit 0
}

$runCli = $Download -or (-not [string]::IsNullOrWhiteSpace($InputPath)) -or (-not [string]::IsNullOrWhiteSpace($Output))
if ($Gui -or -not $runCli) {
    $form = New-FwBuilderForm
    [void]$form.ShowDialog()
    $form.Dispose()
    exit 0
}

try {
    if (!$Output -and $Manifest -eq $bundledManifest -and (Test-Path -LiteralPath $otaDirectory -PathType Container)) {
        $Output = $defaultOtaOutput
    }
    $buildArguments = @{
        ManifestPath = $Manifest
        OutputPath   = $Output
    }
    if (-not [string]::IsNullOrWhiteSpace($Patch)) {
        $buildArguments.PatchPath = $Patch
    }
    if ($Download) {
        $buildArguments.Download = $true
    }
    else {
        $buildArguments.InputPath = $InputPath
    }
    $result = Invoke-RoyBuild @buildArguments
    Write-Output "Вход SHA256: $($result.ImageSha256)"
    Write-Output "Выход SHA256: $($result.OutputSha256)"
    Write-Output "Записано: $($result.OutputPath) ($($result.ByteCount) байт)"
    exit 0
}
catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
