param(
    [Parameter(Mandatory = $true)][string]$CommonLibSource,
    [Parameter(Mandatory = $true)][string]$DependencyPrefix,
    [switch]$SkipChecks
)
$ErrorActionPreference = 'Stop'
$projectPath = Split-Path -Parent $PSScriptRoot
$buildPath = Join-Path $projectPath 'build'
$outputPath = Join-Path $projectPath 'out/release'
$distPath = Join-Path $projectPath 'dist'

& cmake -S $projectPath -B $buildPath -G 'Visual Studio 17 2022' -A x64 "-DCOMMONLIBSSE_SOURCE_DIR=$CommonLibSource" "-DCMAKE_PREFIX_PATH=$DependencyPrefix"
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed' }
& cmake --build $buildPath --config Release --parallel 4
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed' }
if (-not $SkipChecks) {
    & ctest --test-dir $buildPath -C Release --output-on-failure
    if ($LASTEXITCODE -ne 0) { throw 'Bridge integration checks failed' }
}
& cmake --install $buildPath --config Release --component Mod --prefix $outputPath
if ($LASTEXITCODE -ne 0) { throw 'Packaging installation failed' }
New-Item -ItemType Directory -Force $distPath | Out-Null
Compress-Archive -Path (Join-Path $outputPath '*') -DestinationPath (Join-Path $distPath 'TweenMenuSMFBridge-1.0.0-NG.zip') -Force

