param(
    [int]$ProcessId
)

$ErrorActionPreference = 'Stop'
if ($ProcessId -le 0) {
    $targets = @(Get-Process -Name 'AC4BFSP' -ErrorAction Stop)
    if ($targets.Count -ne 1) {
        throw "Pass -ProcessId when zero or multiple AC4BFSP processes are present."
    }
    $ProcessId = $targets[0].Id
}

$name = "Local\AC4ControlObserver.Stop.$ProcessId"
$event = [System.Threading.EventWaitHandle]::OpenExisting($name)
try {
    [void]$event.Set()
    Write-Output "Stop requested for AC4 process $ProcessId."
} finally {
    $event.Dispose()
}
