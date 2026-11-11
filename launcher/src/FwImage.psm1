#Requires -Version 5.1
Set-StrictMode -Version 2.0

$script:JsonSerializer = $null

function Get-JsonSerializer {
    if ($null -eq $script:JsonSerializer) {
        Add-Type -AssemblyName System.Web.Extensions
        $serializer = New-Object System.Web.Script.Serialization.JavaScriptSerializer
        $serializer.MaxJsonLength = 67108864
        $script:JsonSerializer = $serializer
    }
    return $script:JsonSerializer
}

function Read-TextFile {
    param([Parameter(Mandatory = $true)][string]$Path)
    if (-not (Test-Path -LiteralPath $Path)) {
        throw "MANIFEST: файл не найден: $Path"
    }
    $bytes = [System.IO.File]::ReadAllBytes($Path)
    if ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF) {
        return [System.Text.Encoding]::UTF8.GetString($bytes, 3, $bytes.Length - 3)
    }
    return [System.Text.Encoding]::UTF8.GetString($bytes)
}

function Read-JsonDocument {
    param([Parameter(Mandatory = $true)][string]$Path)
    $text = Read-TextFile -Path $Path
    try {
        return (Get-JsonSerializer).DeserializeObject($text)
    }
    catch {
        throw "MANIFEST: не разобран JSON ${Path}: $($_.Exception.Message)"
    }
}

function Get-JsonField {
    param($Document, [Parameter(Mandatory = $true)][string]$Name)
    if ($null -eq $Document) {
        return $null
    }
    if ($Document -is [System.Collections.IDictionary]) {
        foreach ($key in @($Document.Keys)) {
            if ([string]$key -eq $Name) {
                $value = $Document[$key]
                if ($value -is [System.Array]) {
                    return ,$value
                }
                return $value
            }
        }
        return $null
    }
    $property = $Document.PSObject.Properties[$Name]
    if ($null -eq $property) {
        return $null
    }
    if ($property.Value -is [System.Array]) {
        return ,$property.Value
    }
    return $property.Value
}

function ConvertTo-ObjectArray {
    param($Value)
    if ($null -eq $Value) {
        return ,@()
    }
    if ($Value -is [System.Array]) {
        return ,$Value
    }
    return ,@($Value)
}

function Get-Sha256Hex {
    param([Parameter(Mandatory = $true)][byte[]]$Bytes)
    $algorithm = [System.Security.Cryptography.SHA256]::Create()
    try {
        $hash = $algorithm.ComputeHash($Bytes)
        return ([System.BitConverter]::ToString($hash)).Replace('-', '').ToLowerInvariant()
    }
    finally {
        $algorithm.Dispose()
    }
}

function Get-FileSha256Hex {
    param([Parameter(Mandatory = $true)][string]$Path)
    $stream = [System.IO.File]::OpenRead($Path)
    try {
        $algorithm = [System.Security.Cryptography.SHA256]::Create()
        try {
            $hash = $algorithm.ComputeHash($stream)
            return ([System.BitConverter]::ToString($hash)).Replace('-', '').ToLowerInvariant()
        }
        finally {
            $algorithm.Dispose()
        }
    }
    finally {
        $stream.Dispose()
    }
}

function Assert-Sha256Equal {
    param(
        [Parameter(Mandatory = $true)][string]$Actual,
        [Parameter(Mandatory = $true)][string]$Expected,
        [Parameter(Mandatory = $true)][string]$Code,
        [Parameter(Mandatory = $true)][string]$Label
    )
    $normalized = $Expected.Trim().ToLowerInvariant()
    if ($normalized -notmatch '^[0-9a-f]{64}$') {
        throw "MANIFEST: $Label должен быть SHA256 из 64 шестнадцатеричных знаков."
    }
    if ($Actual -ne $normalized) {
        throw "${Code}: $Label не совпал. Ожидался $normalized, получен $Actual."
    }
}

function Read-FwManifest {
    param([Parameter(Mandatory = $true)][string]$Path)
    $document = Read-JsonDocument -Path $Path
    foreach ($name in @('sourceUrl', 'packageSha256', 'packageEntry', 'inputSha256', 'patchFile', 'outputSha256')) {
        $value = Get-JsonField -Document $document -Name $name
        if ($null -eq $value -or [string]$value -eq '') {
            throw "MANIFEST: в манифесте нет поля $name."
        }
    }
    $slice = Get-JsonField -Document $document -Name 'slice'
    $start = Get-JsonField -Document $slice -Name 'start'
    $endInclusive = Get-JsonField -Document $slice -Name 'endInclusive'
    if ($null -eq $start -or $null -eq $endInclusive) {
        throw 'MANIFEST: в slice нужны start и endInclusive.'
    }
    return $document
}

function Read-Alpha13Patch {
    param([Parameter(Mandatory = $true)][string]$Path)
    $document = Read-JsonDocument -Path $Path
    $length = Get-JsonField -Document $document -Name 'length'
    if ($null -eq $length) {
        throw 'PATCH_LENGTH: в alpha13-patch.json нет поля length.'
    }
    $runs = Get-JsonField -Document $document -Name 'runs'
    if ($null -eq $runs) {
        throw 'PATCH_RANGE: в alpha13-patch.json нет поля runs.'
    }
    return $document
}

function Get-RoyPreparationText {
    param([Parameter(Mandatory = $true)]$Manifest)
    $start = [int64](Get-JsonField -Document (Get-JsonField -Document $Manifest -Name 'slice') -Name 'start')
    $endInclusive = [int64](Get-JsonField -Document (Get-JsonField -Document $Manifest -Name 'slice') -Name 'endInclusive')
    $url = [string](Get-JsonField -Document $Manifest -Name 'sourceUrl')
    $patchFile = [string](Get-JsonField -Document $Manifest -Name 'patchFile')
    $entry = [string](Get-JsonField -Document $Manifest -Name 'packageEntry')
    return @"
Подготовка Roy на этой форме автоматическая. Отдельного ручного шага нет.

Кнопка «Собрать» сама выполняет цепочку:
1. Берёт открытый пакет FW189 по адресу $url либо локальный файл, выбранный на форме. Из zip читается запись $entry.
2. Считает SHA256 входного образа и сравнивает его с inputSha256 из локального манифеста. Чужой файл не режется.
3. Копирует байты с индекса $start по индекс $endInclusive включительно. Индексы с нуля. Границы лежат в манифесте, не в коде формы.
4. Применяет $patchFile. Поле length задаёт длину результата и должно совпасть с длиной среза. Поле runs — список правок. У каждой правки есть offset и bytes в Base64. Байты пишутся поверх среза с этого смещения. Поздняя правка перекрывает раннюю.
5. Считает SHA256 результата и сравнивает его с outputSha256. Файл пишется только после совпадения, по пути, выбранному на форме.

Форма не правит шестнадцатеричный дамп и не соединяется с устройством. Смена runs требует нового outputSha256 в манифесте.
"@
}

function Test-ZipPath {
    param([Parameter(Mandatory = $true)][string]$Path)
    $stream = [System.IO.File]::OpenRead($Path)
    try {
        if ($stream.Length -lt 2) {
            return $false
        }
        $first = $stream.ReadByte()
        $second = $stream.ReadByte()
        return ($first -eq 0x50 -and $second -eq 0x4B)
    }
    finally {
        $stream.Dispose()
    }
}

function Read-ZipEntryBytes {
    param(
        [Parameter(Mandatory = $true)][string]$ZipPath,
        [Parameter(Mandatory = $true)][string]$EntryName
    )
    if ($EntryName -match '(^[/\\])|(\.\.)') {
        throw "ENTRY_MISSING: недопустимое имя записи: $EntryName"
    }
    Add-Type -AssemblyName System.IO.Compression
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $archive = [System.IO.Compression.ZipFile]::OpenRead($ZipPath)
    try {
        $entry = $archive.GetEntry($EntryName)
        if ($null -eq $entry) {
            throw "ENTRY_MISSING: в архиве нет записи $EntryName."
        }
        $memory = New-Object System.IO.MemoryStream
        try {
            $source = $entry.Open()
            try {
                $source.CopyTo($memory)
            }
            finally {
                $source.Dispose()
            }
            $buffer = $memory.ToArray()
            return ,$buffer
        }
        finally {
            $memory.Dispose()
        }
    }
    finally {
        $archive.Dispose()
    }
}

function Read-ImageBytes {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$PackageEntry,
        [string]$PackageSha256
    )
    if (Test-ZipPath -Path $Path) {
        if ([string]::IsNullOrWhiteSpace($PackageSha256)) {
            throw 'MANIFEST: для zip нужен packageSha256.'
        }
        $packageHash = Get-FileSha256Hex -Path $Path
        Assert-Sha256Equal -Actual $packageHash -Expected $PackageSha256 -Code 'HASH_PACKAGE' -Label 'SHA256 пакета'
        return Read-ZipEntryBytes -ZipPath $Path -EntryName $PackageEntry
    }
    $raw = [System.IO.File]::ReadAllBytes($Path)
    return ,$raw
}

function New-FirmwareSlice {
    param(
        [Parameter(Mandatory = $true)][byte[]]$Image,
        [Parameter(Mandatory = $true)][int]$Start,
        [Parameter(Mandatory = $true)][int]$EndInclusive
    )
    if ($Start -lt 0 -or $EndInclusive -lt $Start -or $EndInclusive -ge $Image.Length) {
        throw "SLICE_RANGE: срез $Start..$EndInclusive вне образа длиной $($Image.Length)."
    }
    $count = $EndInclusive - $Start + 1
    $slice = New-Object byte[] $count
    [System.Buffer]::BlockCopy($Image, $Start, $slice, 0, $count)
    return ,$slice
}

function Add-Alpha13Patch {
    param(
        [Parameter(Mandatory = $true)][byte[]]$Slice,
        [Parameter(Mandatory = $true)]$Patch
    )
    $length = [int64](Get-JsonField -Document $Patch -Name 'length')
    if ($length -lt $Slice.Length) {
        throw "PATCH_LENGTH: length=$length, длина среза=$($Slice.Length)."
    }
    $output = New-Object byte[] $length
    [System.Buffer]::BlockCopy($Slice, 0, $output, 0, $Slice.Length)
    $runs = ConvertTo-ObjectArray (Get-JsonField -Document $Patch -Name 'runs')
    foreach ($run in $runs) {
        $offsetValue = Get-JsonField -Document $run -Name 'offset'
        $encoded = Get-JsonField -Document $run -Name 'bytes'
        if ($null -eq $offsetValue -or $null -eq $encoded) {
            throw 'PATCH_RANGE: в правке нужны offset и bytes.'
        }
        $offset = [int]$offsetValue
        $cleaned = ([string]$encoded).Trim()
        if ($cleaned.Length -eq 0) {
            throw 'PATCH_RANGE: bytes пустой.'
        }
        try {
            $raw = [Convert]::FromBase64String($cleaned)
        }
        catch {
            throw 'PATCH_RANGE: bytes не является Base64.'
        }
        if ($offset -lt 0 -or ($offset + $raw.Length) -gt $output.Length) {
            throw "PATCH_RANGE: правка offset=$offset длина=$($raw.Length) выходит за length=$($output.Length)."
        }
        [System.Buffer]::BlockCopy($raw, 0, $output, $offset, $raw.Length)
    }
    return ,$output
}

function Save-VerifiedBytes {
    param(
        [Parameter(Mandatory = $true)][byte[]]$Bytes,
        [Parameter(Mandatory = $true)][string]$OutputPath
    )
    $parent = [System.IO.Path]::GetDirectoryName($OutputPath)
    if ([string]::IsNullOrWhiteSpace($parent) -or -not (Test-Path -LiteralPath $parent)) {
        throw "USAGE: каталог для результата не существует: $parent"
    }
    $partial = $OutputPath + '.partial'
    [System.IO.File]::WriteAllBytes($partial, $Bytes)
    try {
        if (Test-Path -LiteralPath $OutputPath) {
            Remove-Item -LiteralPath $OutputPath -Force
        }
        Move-Item -LiteralPath $partial -Destination $OutputPath
    }
    catch {
        if (Test-Path -LiteralPath $partial) {
            Remove-Item -LiteralPath $partial -Force
        }
        throw
    }
}

function Get-PublicPackageFile {
    param(
        [Parameter(Mandatory = $true)][string]$Url,
        [Parameter(Mandatory = $true)][string]$ExpectedSha256,
        [Parameter(Mandatory = $true)][string]$CacheDirectory
    )
    $normalizedUrl = $Url.Trim()
    if ($normalizedUrl -notmatch '^https://') {
        throw 'DOWNLOAD: адрес пакета должен начинаться с https://.'
    }
    if (-not (Test-Path -LiteralPath $CacheDirectory)) {
        New-Item -ItemType Directory -Path $CacheDirectory | Out-Null
    }
    $fileName = [System.IO.Path]::GetFileName(([Uri]$normalizedUrl).AbsolutePath)
    if ([string]::IsNullOrWhiteSpace($fileName)) {
        $fileName = 'FW189.zip'
    }
    $destination = Join-Path $CacheDirectory $fileName
    if (Test-Path -LiteralPath $destination) {
        $cached = Get-FileSha256Hex -Path $destination
        $expected = $ExpectedSha256.Trim().ToLowerInvariant()
        if ($cached -eq $expected) {
            return $destination
        }
        Remove-Item -LiteralPath $destination -Force
    }
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12
    $client = New-Object System.Net.WebClient
    try {
        $client.Headers['User-Agent'] = 'FwImageBuilder/1.0'
        try {
            $client.DownloadFile($normalizedUrl, $destination)
        }
        catch {
            if (Test-Path -LiteralPath $destination) {
                Remove-Item -LiteralPath $destination -Force
            }
            throw "DOWNLOAD: не удалось скачать пакет. $($_.Exception.Message)"
        }
    }
    finally {
        $client.Dispose()
    }
    return $destination
}

function Invoke-RoyBuild {
    param(
        [Parameter(Mandatory = $true)][string]$ManifestPath,
        [string]$PatchPath,
        [string]$InputPath,
        [string]$OutputPath,
        [switch]$Download,
        [string]$CacheDirectory
    )
    if ([string]::IsNullOrWhiteSpace($OutputPath)) {
        throw 'USAGE: укажите файл результата.'
    }
    $hasInput = -not [string]::IsNullOrWhiteSpace($InputPath)
    if ($Download -and $hasInput) {
        throw 'USAGE: укажите либо загрузку FW189, либо локальный файл.'
    }
    if (-not $Download -and -not $hasInput) {
        throw 'USAGE: укажите -Download или локальный файл.'
    }
    $manifest = Read-FwManifest -Path $ManifestPath
    $manifestDirectory = [System.IO.Path]::GetDirectoryName($ManifestPath)
    if ([string]::IsNullOrWhiteSpace($PatchPath)) {
        $patchName = [string](Get-JsonField -Document $manifest -Name 'patchFile')
        if ([System.IO.Path]::IsPathRooted($patchName)) {
            $PatchPath = $patchName
        }
        else {
            $PatchPath = Join-Path $manifestDirectory $patchName
        }
    }
    $patch = Read-Alpha13Patch -Path $PatchPath
    $packageSha = [string](Get-JsonField -Document $manifest -Name 'packageSha256')
    $entry = [string](Get-JsonField -Document $manifest -Name 'packageEntry')
    if ($Download) {
        if ([string]::IsNullOrWhiteSpace($CacheDirectory)) {
            $CacheDirectory = Join-Path $env:TEMP 'fw189-builder'
        }
        $InputPath = Get-PublicPackageFile -Url ([string](Get-JsonField -Document $manifest -Name 'sourceUrl')) -ExpectedSha256 $packageSha -CacheDirectory $CacheDirectory
    }
    elseif (-not (Test-Path -LiteralPath $InputPath)) {
        throw "USAGE: входной файл не найден: $InputPath"
    }
    $image = Read-ImageBytes -Path $InputPath -PackageEntry $entry -PackageSha256 $(if (Test-ZipPath -Path $InputPath) { $packageSha } else { '' })
    $imageHash = Get-Sha256Hex -Bytes $image
    Assert-Sha256Equal -Actual $imageHash -Expected ([string](Get-JsonField -Document $manifest -Name 'inputSha256')) -Code 'HASH_INPUT' -Label 'SHA256 входа'
    $sliceNode = Get-JsonField -Document $manifest -Name 'slice'
    $slice = New-FirmwareSlice -Image $image -Start ([int](Get-JsonField -Document $sliceNode -Name 'start')) -EndInclusive ([int](Get-JsonField -Document $sliceNode -Name 'endInclusive'))
    $built = Add-Alpha13Patch -Slice $slice -Patch $patch
    $outputHash = Get-Sha256Hex -Bytes $built
    Assert-Sha256Equal -Actual $outputHash -Expected ([string](Get-JsonField -Document $manifest -Name 'outputSha256')) -Code 'HASH_OUTPUT' -Label 'SHA256 выхода'
    Save-VerifiedBytes -Bytes $built -OutputPath $OutputPath
    return [pscustomobject]@{
        ImageSha256  = $imageHash
        OutputSha256 = $outputHash
        OutputPath   = $OutputPath
        ByteCount    = $built.Length
        InputPath    = $InputPath
        PatchPath    = $PatchPath
    }
}

Export-ModuleMember -Function @(
    'Read-JsonDocument',
    'Get-JsonField',
    'Get-Sha256Hex',
    'Get-FileSha256Hex',
    'Read-FwManifest',
    'Read-Alpha13Patch',
    'Get-RoyPreparationText',
    'Test-ZipPath',
    'Read-ZipEntryBytes',
    'Read-ImageBytes',
    'New-FirmwareSlice',
    'Add-Alpha13Patch',
    'Invoke-RoyBuild'
)
