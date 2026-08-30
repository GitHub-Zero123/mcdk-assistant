[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"

function Invoke-NativeCommand {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [Parameter(Mandatory = $true)]
        [string[]]$ArgumentList
    )

    & $FilePath @ArgumentList
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code ${LASTEXITCODE}: $FilePath $($ArgumentList -join ' ')"
    }
}

$repoRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")).Path
$buildDir = Join-Path $repoRoot "build\x64-msvc-release"
$dictsDir = Join-Path $repoRoot "dicts"
$knowledgeDir = Join-Path $repoRoot "knowledge"
$solutionsDir = Join-Path $repoRoot "solutions\solutions"
$solutionsSource = Join-Path $repoRoot "solutions\cpp\solution_index.cpp"
$hasSolutions =
    (Test-Path -LiteralPath $solutionsSource -PathType Leaf) -and
    (Test-Path -LiteralPath $solutionsDir -PathType Container)
$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path -LiteralPath $vswhere -PathType Leaf)) {
    throw "vswhere.exe was not found. Install Visual Studio with the Desktop development with C++ workload."
}

$env:Path = "$(Split-Path -Parent $vswhere);$env:Path"

$vsInstallPath = @(
    & $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath
) | Select-Object -First 1

if (-not $vsInstallPath) {
    throw "No Visual Studio installation with the MSVC x64 tools was found."
}

$devShellModule = Join-Path $vsInstallPath "Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
if (-not (Test-Path -LiteralPath $devShellModule -PathType Leaf)) {
    throw "Visual Studio developer shell module was not found: $devShellModule"
}

Import-Module $devShellModule
Enter-VsDevShell -VsInstallPath $vsInstallPath `
    -SkipAutomaticLocation `
    -DevCmdArguments "-arch=x64 -host_arch=x64"

Push-Location $repoRoot
try {
    Invoke-NativeCommand -FilePath "cmake" -ArgumentList @(
        "--preset", "x64-msvc-release"
    )
    Invoke-NativeCommand -FilePath "cmake" -ArgumentList @(
        "--build", "--preset", "x64-msvc-release",
        "--target", "mcdk-index-compiler"
    )

    $compiler = Join-Path $buildDir "tools\mcdk-index-compiler\mcdk-index-compiler.exe"
    if (-not (Test-Path -LiteralPath $compiler -PathType Leaf)) {
        throw "Index compiler was not generated: $compiler"
    }

    $compilerArguments = @(
        "--output-dir", $buildDir,
        "--dicts-dir", $dictsDir,
        "--knowledge-dir", $knowledgeDir
    )
    if ($hasSolutions) {
        $compilerArguments += @("--solutions-dir", $solutionsDir)
    }
    else {
        $staleSolutionsCache = Join-Path $buildDir "mcdk_solutions_cache.bin"
        if (Test-Path -LiteralPath $staleSolutionsCache -PathType Leaf) {
            Remove-Item -LiteralPath $staleSolutionsCache -Force
        }
        Write-Host "Solutions sources are absent; skipped the optional solutions cache."
    }

    Invoke-NativeCommand -FilePath $compiler -ArgumentList $compilerArguments

    $expectedCaches = @(
        "mcdk_index_cache.bin",
        "mcdk_sapi_index_cache.bin"
    )
    if ($hasSolutions) {
        $expectedCaches += "mcdk_solutions_cache.bin"
    }
    $missingCaches = @(
        $expectedCaches | Where-Object {
            -not (Test-Path -LiteralPath (Join-Path $buildDir $_) -PathType Leaf)
        }
    )
    if ($missingCaches.Count -ne 0) {
        throw "Expected cache files were not generated: $($missingCaches -join ', ')"
    }

    Write-Host "Generated index caches in: $buildDir"
    Get-Item -LiteralPath ($expectedCaches | ForEach-Object { Join-Path $buildDir $_ }) |
        Select-Object Name, Length, LastWriteTime |
        Format-Table -AutoSize
}
finally {
    Pop-Location
}
