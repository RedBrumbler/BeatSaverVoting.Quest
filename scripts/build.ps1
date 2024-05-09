param(
    [Parameter(Mandatory=$false)]
    [Switch]$clean
)

$root = "${PSScriptRoot}\.."
$build = "${root}\build"

# if user specified clean, remove all build files
if ($clean.IsPresent)
{
    if (Test-Path -Path ${build})
    {
        remove-item ${build} -R
    }
}

if (($clean.IsPresent) -or (-not (Test-Path -Path ${build})))
{
    new-item -Path ${build} -ItemType Directory
}


& cmake -G "Ninja" -S ${root} -B ${build}
& cmake --build ${build}
