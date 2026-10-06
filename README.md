# batery sentinel
it tells you that you have reached 80% in your mac

# for installing it do this 


sudo mkdir -p /usr/local/bin


sudo cp battery_alert /usr/local/bin/


sudo chmod +x /usr/local/bin/battery_alert


## in case it the OS blocks you
xattr -d com.apple.quarantine /usr/local/bin/battery_alert

## to make it run at start
nano ~/Library/LaunchAgents/com.user.batteryalert.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://apple.com">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.user.batteryalert</string>
    
    <key>ProgramArguments</key>
    <array>
        <string>/usr/local/bin/battery_alert</string>
    </array>
    
    <key>RunAtLoad</key>
    <true/>
    
    <key>KeepAlive</key>
    <true/>
    
    <key>StandardErrorPath</key>
    <string>/tmp/com.user.batteryalert.err</string>
    <key>StandardOutPath</key>
    <string>/tmp/com.user.batteryalert.out</string>
</dict>
</plist>


## then run it
launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/com.user.batteryalert.plist

