$MIX2MONO_install_path = 'C:\Program Files\mix2mono'
$MIX2MONO_exe_link = "https://github.com/ymich9963/mix2mono/releases/download/v0.1.0/mix2mono.exe"

if (Test-Path -Path $MIX2MONO_install_path) {
    Write-Output "Removing previously installed executable."
    Remove-Item $MIX2MONO_install_path -r # rm command
}

mkdir $MIX2MONO_install_path 
curl -fsSLO $MIX2MONO_exe_link
Move-Item mix2mono.exe $MIX2MONO_install_path # mv command
Write-Output "Downloaded executable." # echo command

$Sys_Env_Path_Value = Get-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment\' -Name Path 

# Change the backslashes to frontslashes so that -split can work
$MIX2MONO_install_path_frontslash = $MIX2MONO_install_path -replace "\\","/"
$Sys_Env_Path_Value_frontslash = $Sys_Env_Path_Value.Path -replace "\\", "/"

# Check if the install path exists by splitting the Path variable value
$MIX2MONO_path_check = $Sys_Env_Path_Value_frontslash -split $MIX2MONO_install_path_frontslash | Measure-Object 

if ($MIX2MONO_path_check.Count -igt 1) {
    Write-Output "Detected previous Mix2Mono installation."
    Write-Output "Nothing was added to the system Path variable."
} else {
    Write-Output "Detected no previous Mix2Mono install."
    Write-Output "Adding executable to system Path environment variable."
    $New_Path_Value = $Sys_Env_Path_Value.Path + ";" + $MIX2MONO_install_path + ";" 
    Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment\' -Name Path -Value $New_Path_Value # set the system environment variable for Mix2Mono 
}

Write-Output "Succesfully installed Mix2Mono."

Read-Host -Prompt "Press Enter to exit"

Exit

