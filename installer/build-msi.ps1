#!/usr/bin/env pwsh
# Script to build MSI installer using WiX Toolset
# Usage: .\build-msi.ps1 [-ExePath <path>] [-OutputPath <path>]

param(
    [string]$ExePath = "",
    [string]$OutputPath = ""
)

$ErrorActionPreference = "Stop"

# Get script directory and project root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir

# Set default paths
if ([string]::IsNullOrEmpty($ExePath)) {
    # Try multiple possible locations
    $possiblePaths = @(
        (Join-Path $ProjectRoot "cmake-build-debug\bin\TemplateBuilder.exe"),
        (Join-Path $ProjectRoot "build\bin\Release\TemplateBuilder.exe"),
        (Join-Path $ProjectRoot "build\bin\Debug\TemplateBuilder.exe"),
        (Join-Path $ProjectRoot "build\bin\TemplateBuilder.exe")
    )
    
    $ExePath = $null
    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            $ExePath = $path
            break
        }
    }
    
    if ([string]::IsNullOrEmpty($ExePath)) {
        $ExePath = $possiblePaths[0]  # Default to first if none found
    }
}

if ([string]::IsNullOrEmpty($OutputPath)) {
    $OutputPath = Join-Path $ProjectRoot "TemplateBuilder.msi"
}

# Check if executable exists
if (-not (Test-Path $ExePath)) {
    Write-Error "Executable not found: $ExePath"
    Write-Host "Please build the project first or specify -ExePath parameter"
    exit 1
}

Write-Host "Building MSI Installer"
Write-Host "======================"
Write-Host "Executable: $ExePath"
Write-Host "Output: $OutputPath"
Write-Host ""

# Setup directories
$installerDir = Join-Path $ScriptDir "bin"
$buildDir = Join-Path $ProjectRoot "build-temp"

Write-Host "Creating directories..."
New-Item -ItemType Directory -Force -Path $installerDir | Out-Null
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

# Copy executable to installer directory
$exeDest = Join-Path $installerDir "TemplateBuilder.exe"
Write-Host "Copying executable to $exeDest..."
Copy-Item $ExePath -Destination $exeDest -Force

if (-not (Test-Path $exeDest)) {
    Write-Error "Failed to copy executable"
    exit 1
}

# Copy Terms of Use file to installer directory (needed for WiX)
$termsSource = Join-Path $ScriptDir "TERMS_OF_USE.rtf"
$termsDest = Join-Path $installerDir "TERMS_OF_USE.rtf"
if (Test-Path $termsSource) {
    Write-Host "Copying Terms of Use file to $termsDest..."
    Copy-Item $termsSource -Destination $termsDest -Force
} else {
    Write-Warning "Terms of Use file not found: $termsSource"
}

# Check for dependencies and copy required DLLs
Write-Host "Checking for required DLLs and dependencies..."

# Try to detect executable dependencies using dumpbin (MSVC) or objdump (MinGW/GCC)
$hasMingwDlls = $false
$requiredDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
$exeDir = Split-Path -Parent $ExePath
# Check both 'libs' and 'lib' directories (some setups use 'lib' instead of 'libs')
$libsDir = if (Test-Path (Join-Path $ProjectRoot "libs")) { 
    Join-Path $ProjectRoot "libs" 
} elseif (Test-Path (Join-Path $ProjectRoot "lib")) { 
    Join-Path $ProjectRoot "lib" 
} else { 
    Join-Path $ProjectRoot "libs"  # Default fallback
}
$copiedDlls = @()

foreach ($dll in $requiredDlls) {
    $dllDest = Join-Path $installerDir $dll
    $found = $false
    
    # First, try to find in libs/lib directory (project root)
    if (Test-Path $libsDir) {
        $dllSource = Join-Path $libsDir $dll
        if (Test-Path $dllSource) {
            Copy-Item $dllSource -Destination $dllDest -Force
            $copiedDlls += $dll
            Write-Host "  Copied from ${libsDir}: $dll"
            $found = $true
        }
    }
    
    # Also check the other directory name if different
    $altLibsDir = if ($libsDir -like "*\libs") { Join-Path $ProjectRoot "lib" } else { Join-Path $ProjectRoot "libs" }
    if (-not $found -and (Test-Path $altLibsDir)) {
        $dllSource = Join-Path $altLibsDir $dll
        if (Test-Path $dllSource) {
            Copy-Item $dllSource -Destination $dllDest -Force
            $copiedDlls += $dll
            Write-Host "  Copied from ${altLibsDir}: $dll"
            $found = $true
        }
    }
    
    # If not found in libs, try executable directory
    if (-not $found) {
        $dllSource = Join-Path $exeDir $dll
        if (Test-Path $dllSource) {
            Copy-Item $dllSource -Destination $dllDest -Force
            $copiedDlls += $dll
            Write-Host "  Copied from executable directory: $dll"
            $found = $true
        }
    }
    
    # If still not found, try common MinGW locations
    if (-not $found) {
        $mingwPaths = @(
            "$env:ProgramFiles\mingw64\bin",
            "$env:ProgramFiles(x86)\mingw64\bin",
            "C:\mingw64\bin",
            "C:\msys64\mingw64\bin",
            "C:\msys64\usr\bin"
        )
        
        foreach ($mingwPath in $mingwPaths) {
            if (Test-Path $mingwPath) {
                $dllCandidate = Join-Path $mingwPath $dll
                if (Test-Path $dllCandidate) {
                    Copy-Item $dllCandidate -Destination $dllDest -Force
                    $copiedDlls += $dll
                    Write-Host "  Copied from MinGW: $dll"
                    $found = $true
                    break
                }
            }
        }
    }
}

if ($copiedDlls.Count -gt 0) {
    Write-Host "MinGW/GCC runtime DLLs copied: $($copiedDlls -join ', ')" -ForegroundColor Green
    $hasMingwDlls = $true
    $needsVCRedist = $false
} else {
    Write-Host "No MinGW/GCC runtime DLLs found. Assuming MSVC-compiled executable." -ForegroundColor Yellow
    $needsVCRedist = $true
}

# Always check for MSVC runtime DLLs (they might be in lib/libs even if MinGW DLLs are also present)
Write-Host "Checking for MSVC runtime DLLs..." -ForegroundColor Cyan
$msvcRuntimeDlls = @(
    "VCRUNTIME140.dll",
    "VCRUNTIME140_1.dll",
    "MSVCP140.dll",
    "MSVCP140_1.dll",
    "MSVCP140_2.dll",
    "VCOMP140.dll"
)

$msvcDllSources = @(
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\*\x64\Microsoft.VC143.CRT\*",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\VC\Redist\MSVC\*\x64\Microsoft.VC143.CRT\*",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\VC\Redist\MSVC\*\x64\Microsoft.VC143.CRT\*",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\*\x64\Microsoft.VC143.CRT\*",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Professional\VC\Redist\MSVC\*\x64\Microsoft.VC143.CRT\*",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Enterprise\VC\Redist\MSVC\*\x64\Microsoft.VC143.CRT\*",
    "${env:ProgramFiles}\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\*\x64\Microsoft.VC142.CRT\*",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\*\x64\Microsoft.VC142.CRT\*",
    "${env:SystemRoot}\System32\*",
    "${env:SystemRoot}\SysWOW64\*"
)

$msvcDllsCopied = @()
foreach ($dll in $msvcRuntimeDlls) {
    $dllDest = Join-Path $installerDir $dll
    $found = $false
    
    # Check if DLL already exists in installer directory
    if (Test-Path $dllDest) {
        $found = $true
        Write-Host "  DLL already exists: $dll"
    } else {
        # First, try to find in libs/lib directory (project root)
        if (Test-Path $libsDir) {
            $dllSource = Join-Path $libsDir $dll
            if (Test-Path $dllSource) {
                Copy-Item $dllSource -Destination $dllDest -Force
                $msvcDllsCopied += $dll
                Write-Host "  Copied from ${libsDir}: $dll" -ForegroundColor Green
                $found = $true
            }
        }
        
        # Also check the other directory name if different
        $altLibsDir = if ($libsDir -like "*\libs") { Join-Path $ProjectRoot "lib" } else { Join-Path $ProjectRoot "libs" }
        if (-not $found -and (Test-Path $altLibsDir)) {
            $dllSource = Join-Path $altLibsDir $dll
            if (Test-Path $dllSource) {
                Copy-Item $dllSource -Destination $dllDest -Force
                $msvcDllsCopied += $dll
                Write-Host "  Copied from ${altLibsDir}: $dll" -ForegroundColor Green
                $found = $true
            }
        }
    }
    
    if (-not $found) {
        # Search in Visual Studio redist directories
        foreach ($pattern in $msvcDllSources) {
            $foundFiles = Get-ChildItem -Path $pattern -Filter $dll -ErrorAction SilentlyContinue | Select-Object -First 1
            if ($foundFiles) {
                Copy-Item $foundFiles.FullName -Destination $dllDest -Force
                $msvcDllsCopied += $dll
                Write-Host "  Copied from Visual Studio: $dll" -ForegroundColor Green
                $found = $true
                break
            }
        }
        
        # If not found, try to copy from System32/SysWOW64
        if (-not $found) {
            $systemPaths = @("${env:SystemRoot}\System32", "${env:SystemRoot}\SysWOW64")
            foreach ($sysPath in $systemPaths) {
                $sysDllPath = Join-Path $sysPath $dll
                if (Test-Path $sysDllPath) {
                    Copy-Item $sysDllPath -Destination $dllDest -Force
                    $msvcDllsCopied += $dll
                    Write-Host "  Copied from Windows: $dll" -ForegroundColor Green
                    $found = $true
                    break
                }
            }
        }
    }
}

if ($msvcDllsCopied.Count -gt 0) {
    Write-Host "MSVC runtime DLLs copied: $($msvcDllsCopied -join ', ')" -ForegroundColor Green
} else {
    if (-not $hasMingwDlls) {
        Write-Host "MSVC runtime DLLs not found in common locations." -ForegroundColor Yellow
        Write-Host "The VC++ Redistributable should provide these DLLs at runtime." -ForegroundColor Yellow
    }
}

# Store flag for WiX build
$env:HasMingwDlls = if ($hasMingwDlls) { "1" } else { "0" }

# Find WiX Toolset
Write-Host "Looking for WiX Toolset..."
$wixPaths = @(
    "${env:ProgramFiles}\WiX Toolset v6\bin",
    "${env:ProgramFiles}\WiX Toolset v3.11\bin",
    "${env:ProgramFiles(x86)}\WiX Toolset v3.11\bin",
    "${env:ChocolateyInstall}\bin"
)

# Try WiX v6 first (wix.exe)
$wixPath = Get-Command wix.exe -ErrorAction SilentlyContinue
$wixExe = $null
$candle = $null
$light = $null
$useWixV6 = $false

if ($wixPath) {
    $wixExe = $wixPath.Source
    $useWixV6 = $true
    Write-Host "Found WiX v6 in PATH: $wixExe"
} else {
    # Try WiX v3.11 (candle.exe and light.exe)
    $candlePath = Get-Command candle.exe -ErrorAction SilentlyContinue
    $lightPath = Get-Command light.exe -ErrorAction SilentlyContinue

    if ($candlePath -and $lightPath) {
        $candle = $candlePath.Source
        $light = $lightPath.Source
        $useWixV6 = $false
        Write-Host "Found WiX v3.11 in PATH: $candle"
    } else {
        # Search in common installation paths
        foreach ($path in $wixPaths) {
            $candidateWix = Join-Path $path "wix.exe"
            $candidateCandle = Join-Path $path "candle.exe"
            $candidateLight = Join-Path $path "light.exe"
            
            if (Test-Path $candidateWix) {
                $wixExe = $candidateWix
                $useWixV6 = $true
                Write-Host "Found WiX v6 at: $path"
                break
            } elseif ((Test-Path $candidateCandle) -and (Test-Path $candidateLight)) {
                $candle = $candidateCandle
                $light = $candidateLight
                $useWixV6 = $false
                Write-Host "Found WiX v3.11 at: $path"
                break
            }
        }
    }
}

if (-not $wixExe -and -not ($candle -and $light)) {
    Write-Warning "WiX Toolset not found."
    Write-Host ""
    Write-Host "Please install WiX Toolset using one of the following methods:"
    Write-Host ""
    Write-Host "Option 1 - Install via Chocolatey (recommended):"
    Write-Host "  choco install wixtoolset -y"
    Write-Host ""
    Write-Host "Option 2 - Download installer:"
    Write-Host "  https://wixtoolset.org/releases/"
    Write-Host ""
    Write-Host "After installation, run this script again."
    Write-Host ""
    
    # Check if Chocolatey is available
    $chocoAvailable = Get-Command choco -ErrorAction SilentlyContinue
    if ($chocoAvailable) {
        $response = Read-Host "Do you want to install WiX Toolset now using Chocolatey? (Y/N)"
        if ($response -eq 'Y' -or $response -eq 'y') {
            Write-Host "Installing WiX Toolset via Chocolatey..."
            choco install wixtoolset -y
            
            if ($LASTEXITCODE -eq 0) {
                Write-Host "WiX Toolset installed successfully. Refreshing PATH..."
                $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
                
                # Try finding WiX again (check for both v6 and v3.11)
                $wixPathNew = Get-Command wix.exe -ErrorAction SilentlyContinue
                $candlePath = Get-Command candle.exe -ErrorAction SilentlyContinue
                $lightPath = Get-Command light.exe -ErrorAction SilentlyContinue
                
                if ($wixPathNew) {
                    $wixExe = $wixPathNew.Source
                    $useWixV6 = $true
                    Write-Host "WiX v6 found: $wixExe" -ForegroundColor Green
                } elseif ($candlePath -and $lightPath) {
                    $candle = $candlePath.Source
                    $light = $lightPath.Source
                    $useWixV6 = $false
                    Write-Host "WiX v3.11 found: $candle" -ForegroundColor Green
                } else {
                    Write-Warning "WiX installed but not found in PATH. Please restart your terminal and run the script again."
                    exit 1
                }
            } else {
                Write-Error "Failed to install WiX Toolset"
                exit 1
            }
        } else {
            exit 1
        }
    } else {
        exit 1
    }
}

# Build MSI using WiX
$wxsFile = Join-Path $ScriptDir "TemplateBuilder.wxs"

# Create a modified WXS file that only includes DLLs if they exist
Write-Host "Preparing WiX source file..."
$wxsContent = Get-Content $wxsFile -Raw

# Check which DLLs actually exist in installer directory
$existingDlls = @()
$dllMapping = @{
    "libgcc_s_seh-1.dll" = "LibGccDll"
    "libstdc++-6.dll" = "LibStdCppDll"
    "libwinpthread-1.dll" = "LibWinPthreadDll"
}

# MSVC runtime DLLs mapping
$msvcDllMapping = @{
    "VCRUNTIME140.dll" = "VCRuntime140Dll"
    "VCRUNTIME140_1.dll" = "VCRuntime140_1Dll"
    "MSVCP140.dll" = "MSVCP140Dll"
    "MSVCP140_1.dll" = "MSVCP140_1Dll"
    "MSVCP140_2.dll" = "MSVCP140_2Dll"
    "VCOMP140.dll" = "VCOMP140Dll"
}

$existingMsvcDlls = @()

foreach ($dllFile in $dllMapping.Keys) {
    $dllPath = Join-Path $installerDir $dllFile
    if (Test-Path $dllPath) {
        $existingDlls += $dllMapping[$dllFile]
        Write-Host "  MinGW DLL found: $dllFile"
    }
}

foreach ($dllFile in $msvcDllMapping.Keys) {
    $dllPath = Join-Path $installerDir $dllFile
    if (Test-Path $dllPath) {
        $existingMsvcDlls += $msvcDllMapping[$dllFile]
        Write-Host "  MSVC DLL found: $dllFile"
    }
}

# If no MinGW DLLs exist, remove the MinGWDlls component from WXS
if (-not $hasMingwDlls -or $existingDlls.Count -eq 0) {
    Write-Host "  Removing MinGW DLL component from WXS (DLLs not found or not needed)"
    # Remove the entire MinGWDlls component block including comment
    $pattern = '(?s)\s*<!-- MinGW/GCC runtime DLLs.*?</Component>\s*'
    $wxsContent = $wxsContent -replace $pattern, ''
} else {
    # Build list of DLL file elements to keep
    $dllElements = @()
    foreach ($dllFile in $dllMapping.Keys) {
        $dllId = $dllMapping[$dllFile]
        $dllPath = Join-Path $installerDir $dllFile
        if (Test-Path $dllPath) {
            $dllElements += "                <File Id=`"$dllId`" Source=`"`$(var.SourceDir)\$dllFile`" />"
        }
    }
    
    # Replace the MinGWDlls component with only existing DLLs
    if ($dllElements.Count -gt 0) {
        $newComponentContent = "            <!-- MinGW/GCC runtime DLLs (copied by build-msi.ps1 if the executable depends on them) -->`r`n            <Component Id=`"MinGWDlls`" Guid=`"A2B3C4D5-E6F7-4A8B-9C0D-1E2F3A4B5C6D`">`r`n" + ($dllElements -join "`r`n") + "`r`n            </Component>"
        $pattern = '(?s)\s*<!-- MinGW/GCC runtime DLLs.*?</Component>\s*'
        $wxsContent = $wxsContent -replace $pattern, $newComponentContent
    } else {
        # No DLLs exist, remove component
        $pattern = '(?s)\s*<!-- MinGW/GCC runtime DLLs.*?</Component>\s*'
        $wxsContent = $wxsContent -replace $pattern, ''
    }
}

# Add MSVC runtime DLLs component if they exist
if ($existingMsvcDlls.Count -gt 0) {
    Write-Host "  Adding MSVC runtime DLLs component to WXS"
    $msvcDllElements = @()
    foreach ($dllFile in $msvcDllMapping.Keys) {
        $dllId = $msvcDllMapping[$dllFile]
        $dllPath = Join-Path $installerDir $dllFile
        if (Test-Path $dllPath) {
            $msvcDllElements += "                <File Id=`"$dllId`" Source=`"`$(var.SourceDir)\$dllFile`" />"
        }
    }
    
    if ($msvcDllElements.Count -gt 0) {
        $msvcComponentContent = "`r`n            <!-- MSVC runtime DLLs (copied by build-msi.ps1 if the executable depends on them) -->`r`n            <Component Id=`"MSVCDlls`" Guid=`"B3C4D5E6-F7A8-5B9C-0D1E-2F3A4B5C6D7E`">`r`n" + ($msvcDllElements -join "`r`n") + "`r`n            </Component>"
        
        # Insert before closing ProductComponents ComponentGroup (after ApplicationFiles or MinGWDlls if present)
        $insertPattern = '(</Component>\s*)(</ComponentGroup>\s*<ComponentGroup Id="EnvironmentComponents)'
        if ($wxsContent -match $insertPattern) {
            $wxsContent = $wxsContent -replace $insertPattern, "`$1$msvcComponentContent`r`n            `$2"
        } else {
            # Fallback: try to insert before closing ProductComponents ComponentGroup
            $insertPattern2 = '(<ComponentGroup Id="ProductComponents"[^>]*>.*?</Component>\s*)(</ComponentGroup>\s*<ComponentGroup Id="EnvironmentComponents)'
            if ($wxsContent -match $insertPattern2) {
                $wxsContent = $wxsContent -replace $insertPattern2, "`$1$msvcComponentContent`r`n            `$2"
            } else {
                # Last resort: insert before EnvironmentComponents
                $wxsContent = $wxsContent -replace '(<ComponentGroup Id="EnvironmentComponents)', "$msvcComponentContent`r`n`$1"
            }
        }
    }
}

# Save modified WXS to temporary file
$wxsTempFile = Join-Path $buildDir "TemplateBuilder.wxs"
$wxsContent | Set-Content -Path $wxsTempFile -NoNewline
$wxsFile = $wxsTempFile

Write-Host ""
if ($useWixV6) {
    # WiX v6: Use wix.exe build
    Write-Host "Building MSI with WiX v6..."
    Write-Host "  Input: $wxsFile"
    Write-Host "  Output: $OutputPath"
    
    $OutputDir = Split-Path -Parent $OutputPath
    if (-not [string]::IsNullOrEmpty($OutputDir) -and -not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
    }
    
    # WiX v6 command syntax: wix build -arch x64 -define SourceDir="path" -o output.msi input.wxs
    Write-Host "Executing: $wixExe build -arch x64 -define SourceDir=$installerDir -o $OutputPath $wxsFile"
    
    & $wixExe build -arch x64 -define "SourceDir=$installerDir" -o "$OutputPath" "$wxsFile" 2>&1 | Tee-Object -Variable wixOutput
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "WiX build failed with exit code $LASTEXITCODE"
        Write-Host "WiX output:" -ForegroundColor Red
        Write-Host $wixOutput
        exit 1
    }
} else {
    # WiX v3.11: Use candle.exe and light.exe
    $wixobjPath = Join-Path $buildDir "TemplateBuilder.wixobj"
    
    Write-Host "Compiling WiX source file..."
    Write-Host "  Input: $wxsFile"
    Write-Host "  Output: $wixobjPath"
    
    & $candle "$wxsFile" "-dSourceDir=$installerDir" "-out" "$buildDir\"
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Candle failed with exit code $LASTEXITCODE"
        exit 1
    }
    
    if (-not (Test-Path $wixobjPath)) {
        Write-Error "WixObj file not created at: $wixobjPath"
        exit 1
    }
    
    Write-Host ""
    Write-Host "Linking MSI..."
    Write-Host "  Output: $OutputPath"
    
    $OutputDir = Split-Path -Parent $OutputPath
    if (-not [string]::IsNullOrEmpty($OutputDir) -and -not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
    }
    
    & $light "$wixobjPath" "-out" "$OutputPath"
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Light failed with exit code $LASTEXITCODE"
        exit 1
    }
}

# Verify MSI was created
if (Test-Path $OutputPath) {
    $msiInfo = Get-Item $OutputPath
    Write-Host ""
    Write-Host "MSI created successfully!" -ForegroundColor Green
    Write-Host "  Path: $OutputPath"
    Write-Host "  Size: $([math]::Round($msiInfo.Length / 1KB, 2)) KB"
    
    # Also copy to installer/bin for consistency
    $installerMsi = Join-Path $installerDir "TemplateBuilder.msi"
    Copy-Item $OutputPath -Destination $installerMsi -Force
    Write-Host "  Also copied to: $installerMsi"
} else {
    Write-Error "MSI file not found after build"
    exit 1
}

# Build Bootstrapper with VC++ Redistributable if needed
if ($needsVCRedist) {
    Write-Host ""
    Write-Host "Building Bootstrapper with Visual C++ Redistributable..." -ForegroundColor Cyan
    
    # Look for VC++ Redistributable in lib/libs directory first, then installer directory, then download
    $vcRedistUrl = "https://aka.ms/vs/17/release/vc_redist.x64.exe"
    $vcRedistFileNames = @("vcredist_x64.exe", "vc_redist.x64.exe", "VC_redist.x64.exe")
    $vcRedistPath = $null
    
    # First, check in lib/libs directory
    foreach ($fileName in $vcRedistFileNames) {
        $testPath = Join-Path $libsDir $fileName
        if (Test-Path $testPath) {
            $vcRedistPath = $testPath
            Write-Host "  Found VC++ Redistributable in ${libsDir}: $fileName" -ForegroundColor Green
            break
        }
        
        # Also check alternative lib directory
        $altLibsDir = if ($libsDir -like "*\libs") { Join-Path $ProjectRoot "lib" } else { Join-Path $ProjectRoot "libs" }
        if (Test-Path $altLibsDir) {
            $testPath = Join-Path $altLibsDir $fileName
            if (Test-Path $testPath) {
                $vcRedistPath = $testPath
                Write-Host "  Found VC++ Redistributable in ${altLibsDir}: $fileName" -ForegroundColor Green
                break
            }
        }
    }
    
    # If not found in lib/libs, check installer directory
    if (-not $vcRedistPath) {
        foreach ($fileName in $vcRedistFileNames) {
            $testPath = Join-Path $installerDir $fileName
            if (Test-Path $testPath) {
                $vcRedistPath = $testPath
                Write-Host "  Using existing VC++ Redistributable in installer directory: $fileName" -ForegroundColor Green
                break
            }
        }
    }
    
    # If still not found, try to download
    if (-not $vcRedistPath) {
        $vcRedistPath = Join-Path $installerDir "vc_redist.x64.exe"
        Write-Host "Downloading Visual C++ Redistributable..." -ForegroundColor Yellow
        try {
            $ProgressPreference = 'SilentlyContinue'
            Invoke-WebRequest -Uri $vcRedistUrl -OutFile $vcRedistPath -UseBasicParsing
            Write-Host "  Downloaded: $vcRedistPath" -ForegroundColor Green
        } catch {
            Write-Warning "Failed to download VC++ Redistributable: $_"
            Write-Host "You can manually download it from: $vcRedistUrl"
            Write-Host "Place it in: $libsDir (or $installerDir)"
            Write-Host "Skipping bootstrapper creation..." -ForegroundColor Yellow
            $needsVCRedist = $false
        }
    }
    
    if ($needsVCRedist -and (Test-Path $vcRedistPath)) {
        # Extract version from existing WXS or use default
        $version = "0.1.0"
        $originalWxs = Join-Path $ScriptDir "TemplateBuilder.wxs"
        if (Test-Path $originalWxs) {
            $wxsContent = Get-Content $originalWxs -Raw
            if ($wxsContent -match 'Version="([^"]+)"') {
                $version = $matches[1]
            }
        }
        
        # Create Bundle WXS file
        $bundleWxsFile = Join-Path $ScriptDir "TemplateBuilder.Bundle.wxs"
        $bundleWxsContent = @"
<?xml version="1.0" encoding="UTF-8"?>
<Wix xmlns="http://wixtoolset.org/schemas/v4/wxs"
     xmlns:bal="http://wixtoolset.org/schemas/v4/wxs/bal">
    <Bundle Name="Template Builder $version"
            Version="$version"
            Manufacturer="Template Builder"
            UpgradeCode="B3D6C3F4-9F5B-5G2E-0D4C-8B7F6E5D4C3B">
        <bal:WixStandardBootstrapperApplication />
        
        <Chain>
            <!-- Visual C++ 2015-2022 Redistributable (x64) -->
            <ExePackage Id="VCRedist_x64"
                       DisplayName="Visual C++ 2015-2022 Redistributable (x64)"
                       Cache="no"
                       Compressed="yes"
                       PerMachine="yes"
                       Permanent="yes"
                       Vital="yes"
                       SourceFile="$vcRedistPath"
                       DetectCondition="Installed"
                       InstallCommand="/quiet /norestart" />
            
            <!-- Main MSI package -->
            <MsiPackage SourceFile="$installerMsi"
                       ForcePerMachine="yes"
                       Visible="yes" />
        </Chain>
    </Bundle>
</Wix>
"@
        
        $bundleWxsContent | Set-Content -Path $bundleWxsFile -NoNewline
        
        # Build the bootstrapper
        $bundleOutputPath = $OutputPath -replace '\.msi$', '.exe'
        $bundleOutputPath = Join-Path (Split-Path -Parent $OutputPath) "TemplateBuilder-Setup.exe"
        
        Write-Host "Building bootstrapper..." -ForegroundColor Cyan
        Write-Host "  Input: $bundleWxsFile"
        Write-Host "  Output: $bundleOutputPath"
        
        if ($useWixV6) {
            # WiX v6: Use wix.exe build for bundle
            & $wixExe build -arch x64 -o "$bundleOutputPath" "$bundleWxsFile" 2>&1 | Tee-Object -Variable bundleOutput
            
            if ($LASTEXITCODE -ne 0) {
                Write-Warning "Bootstrapper build failed with exit code $LASTEXITCODE"
                Write-Host "Bundle output:" -ForegroundColor Yellow
                Write-Host $bundleOutput
                Write-Host "MSI installer is still available at: $OutputPath" -ForegroundColor Yellow
            } else {
                if (Test-Path $bundleOutputPath) {
                    $bundleInfo = Get-Item $bundleOutputPath
                    Write-Host ""
                    Write-Host "Bootstrapper created successfully!" -ForegroundColor Green
                    Write-Host "  Path: $bundleOutputPath"
                    Write-Host "  Size: $([math]::Round($bundleInfo.Length / 1MB, 2)) MB"
                    Write-Host ""
                    Write-Host "This bootstrapper includes:" -ForegroundColor Cyan
                    Write-Host "  - Visual C++ Redistributable installer"
                    Write-Host "  - Template Builder MSI installer"
                }
            }
        } else {
            # WiX v3.11: Use candle and light for bundle
            Write-Warning "WiX v3.11 bundle support requires additional steps. Building MSI only."
            Write-Host "To create a bundle with WiX v3.11, you may need WiX Standard Bootstrapper Application."
            Write-Host "MSI installer is available at: $OutputPath"
        }
    }
}
