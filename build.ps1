$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $root "build"
$sourceDir = Join-Path $root "src"
$sources = Get-ChildItem -Path $sourceDir -Filter "*.cpp" | ForEach-Object { $_.FullName }
$output = Join-Path $buildDir "FlappyBirdCont.exe"

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

g++ -std=c++17 -O2 -Wall -Wextra $sources -o "$output" -municode -mwindows -lgdi32 -luser32 -lwinmm

Write-Host "Built: $output"
Write-Host "Run it with: .\build\FlappyBirdCont.exe"
