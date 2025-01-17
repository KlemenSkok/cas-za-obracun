
# Testing

Sending test UDP packets from Windows:
```powershell
$udpClient = New-Object System.Net.Sockets.UdpClient
$serverIp = "0.0.0.0"
$port = 21924
$message = "Pozdravljen server!"
$bytes = [System.Text.Encoding]::UTF8.GetBytes($message)
$udpClient.Send($bytes, $bytes.Length, $serverIp, $port)
$udpClient.Close()
```