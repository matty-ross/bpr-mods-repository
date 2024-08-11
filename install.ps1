$Configurations = (
    "Debug x86",
    "Release x86"
)

Write-Host -Object "Choose configuration:"
for ($i = 0; $i -lt $Configurations.Length; ++$i)
{
    Write-Host -Object "$($i + 1) - $($Configurations[$i])"
}

$Choice = Read-Host -Prompt ">"
$Configuration = $Configurations[$Choice - 1]

$BprDirectory = Get-ItemPropertyValue -Path "HKLM:\SOFTWARE\WOW6432Node\Criterion\BurnoutPR\" -Name "Install Dir"
$BprModsDirectory = $BprDirectory + "mods\"

Copy-Item -Path ".\libraries\mod-manager\bin\$Configuration\mod-manager.dll" -Destination $BprDirectory
Copy-Item -Path ".\vendor\imgui\bin\$Configuration\imgui.dll" -Destination $BprDirectory
Copy-Item -Path ".\mods\*\bin\$Configuration\*.dll" -Destination $BprModsDirectory

Write-Host -Object "Installation finished."
Read-Host -Prompt "Press Enter to quit..." | Out-Null
