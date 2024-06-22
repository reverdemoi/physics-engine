# compile.ps1
$includeDir = ".\headers"
$includeSDLDir = ".\include"
$libDir = ".\lib"
$fileName = "main"

# List of source files
$sourceFiles = @(
    ".\source\main.c",
    ".\source\vector.c",
    ".\source\SDL_utils.c",
    ".\source\ball.c",
    ".\source\common.c"
)

# Compile the source files
gcc -o $fileName $sourceFiles -I"$includeDir" -I"$includeSDLDir" -L"$libDir" -lmingw32 -lSDL2main -lSDL2 -lm

# Check if the gcc command was successful
if ($LASTEXITCODE -eq 0) {
    Write-Output "Compilation successful. Running $fileName.exe..."
    & ".\$fileName.exe"
} else {
    Write-Output "Compilation failed. $fileName.exe will not be run."
}
