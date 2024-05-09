

param(
    [Parameter(Mandatory=$false)]
    [Switch]$log
)

& ${PSScriptRoot}/build.ps1

& adb push build/libbeatsavervoting.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libbeatsavervoting.so

& adb shell am force-stop com.beatgames.beatsaber
& adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
Start-Sleep -Milliseconds 100
& adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
if ($log.IsPresent) {
    & $PSScriptRoot/log.ps1
}
