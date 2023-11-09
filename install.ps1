function Get-Configuration
{
    $Configurations = (
        "Debug x86",
        "Release x86"
    )
    
    $Prompt = "Choose configuration:`n"
    for ($i = 0; $i -lt $Configurations.Length; ++$i)
    {
        $Prompt += "$i. " + $Configurations[$i] + "`n";
    }
    $Choice = Read-Host -Prompt $Prompt
    
    return $Configurations[$Choice]
}


$Configuration = Get-Configuration
$BprDirectory = Get-ItemPropertyValue -Path "HKLM:\SOFTWARE\WOW6432Node\Criterion\BurnoutPR\" -Name "Install Dir"

Copy-Item -Path ".\other\mod-manager\bin\$Configuration\mod-manager.dll" -Destination $BprDirectory
Copy-Item -Path ".\vendor\imgui\bin\$Configuration\imgui.dll" -Destination $BprDirectory
Copy-Item -Path ".\mods\*\bin\$Configuration\*.dll" -Destination ($BprDirectory + "mods\")

Write-Host -Object "Installation finished."