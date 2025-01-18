
# Testing

Sending test UDP packets from Windows:
```powershell
$udpClient = New-Object System.Net.Sockets.UdpClient
$serverIp = "192.168.64.110"
$port = 42069
$message = "Pozdravljen server!"
$bytes = [System.Text.Encoding]::UTF8.GetBytes($message)
$udpClient.Send($bytes, $bytes.Length, $serverIp, $port)
$udpClient.Close()
```

Wireshark filter: `ip.addr == 192.168.64.110 && udp && (udp.port == 42069 || udp.port == 42068)`