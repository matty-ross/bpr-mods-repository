$Configuration = "Debug x86"
#$Configuration = "Release x86"

$BprDirectory = Get-ItemPropertyValue -Path "HKLM:\SOFTWARE\WOW6432Node\Criterion\BurnoutPR\" -Name "Install Dir"

Copy-Item -Path ".\other\mod-manager\bin\$Configuration\mod-manager.dll" -Destination $BprDirectory
Copy-Item -Path ".\vendor\imgui\bin\$Configuration\imgui.dll" -Destination $BprDirectory
Copy-Item -Path ".\mods\*\bin\$Configuration\*.dll" -Destination ($BprDirectory + "mods\")

Write-Host -Object "Installation finished."