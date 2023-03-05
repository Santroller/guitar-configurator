$headers = @{
    Authorization="Bearer $Env:TOKEN"
}
$ver=Invoke-RestMethod -Headers $headers -Uri 'https://api.github.com/repos/sanjay900/guitar-configurator/tags' | % { $_.name}  | sort -Descending {[version] $_.Substring(1)} | Select-Object -first 1
(gc installer\config\config.xml) -replace '1.0.0', $ver | Out-File -encoding ASCII installer\config\config.xml
(gc installer\packages\net.tangentmc.guitar_configurator\meta\package.xml) -replace '1.0.0', $ver | Out-File -encoding ASCII installer\packages\net.tangentmc.guitar_configurator\meta\package.xml
(gc installer\packages\net.tangentmc.guitar_configurator\meta\package.xml) -replace '2010-09-21', (Get-Date -Format "yyy-MM-dd") | Out-File -encoding ASCII installer\packages\net.tangentmc.guitar_configurator\meta\package.xml
