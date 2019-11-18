#<#
$logfilepath = "C:\Utilities\PunchClock-$(Get-Date -UFormat '%y%m').csv";

#pairing
$tries = 1; $maxtries = 2; $portfound = $false;
while($tries -le $maxtries -and -not $portfound){
    foreach($portname in [System.IO.Ports.SerialPort]::getportnames()){
        $portname
        $port = New-Object System.IO.Ports.SerialPort
        $port.PortName = $portname;
        $port.BaudRate = "9600"; $port.Parity = "None";
        $port.DataBits = 8; $port.StopBits = 1; $port.DtrEnable = "true";
        $port.ReadTimeout = 2000
        try{

        $port.open()
        Start-Sleep 3 # wait 2 seconds until Arduino is ready
        
            while($serialinput = $port.ReadLine()){
                "< $serialinput"
                if($serialinput -eq "it you?" + [char]13){
                    $port.Write("it me!"); "> it me!"
                    
                    $portfound = $true;
                    $port.Close() #closes serial connection
                    break;
                }
            }
            if($portfound){
                break;
            }
        }
        catch [TimeoutException]{
            "timed out`n";#$_.Exception.GetType().FullName
        }
        catch [UnauthorizedAccessException]{
            "not authorized for port: $portname. The port may be in use.";
        }
        catch{
            $_.Exception
        }
        finally{
            $port.Close() #closes serial connection
        }
    }
    $tries++;
}

#connect to device and listen for responses
"`nPort found: $portfound"
"Port name : $portname`n"
if($portfound){
    try{
        $port = New-Object System.IO.Ports.SerialPort
        $port.PortName = $portname
        $port.BaudRate = "9600"
        $port.Parity = "None"
        $port.DataBits = 8
        $port.StopBits = 1
        $port.ReadTimeout = 10000 #10 seconds
        $port.DtrEnable = "true"
        $port.open() #opens serial connection

        Start-Sleep 2 # wait 2 seconds until Arduino is ready
        "ready`n"
        try
        {
          while($serialinput = $port.ReadLine()){
            "< $serialinput"
            if($serialinput -eq "boop" + [char]13){
                $info = [PSCustomObject] @{Date=(Get-Date -UFormat "%D"); Time=(Get-Date -UFormat "%T"); User=$env:USERNAME;}
                try{
                    $info | Export-CSV -Path $logfilepath -NoTypeInformation -Append -ErrorAction Stop
                    $port.Write("beep"); "> beep"
                }
                catch{
                    $_.Exception
                    $port.Write("nope"); "> nope"
                }
            }
            if($serialinput -eq "it you?" + [char]13){
                $port.Write("it me!"); "> it me!"
            }
            #$serialinput.ToCharArray() | %{"$_ : $([byte][char]$_)"}
          }
        }
        catch [TimeoutException]{
            "timed out"
        }
    }
    finally{
        $port.Close();
        Write-Host "port closed"
    }
}
