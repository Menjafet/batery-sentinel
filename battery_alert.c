#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
// clang main.c -o sentinel
// this compiles for both;
// clang -target arm64-apple-macos11 -target x86_64-apple-macos10.16 -framework CoreFoundation -framework IOKit battery_alert.c -o battery_alert

static int notification_sent = 0;

void check_battery_and_notify() {
    // capture state
    CFTypeRef power_info = IOPSCopyPowerSourcesInfo();
    if (!power_info) return;

    CFArrayRef power_sources = IOPSCopyPowerSourcesList(power_info);
    if (!power_sources || CFArrayGetCount(power_sources) == 0) {

        if (power_sources) CFRelease(power_sources);
        CFRelease(power_info);
        return;
    }
    // to get uinfo on the main internal battery
    CFDictionaryRef description = IOPSGetPowerSourceDescription(power_info, CFArrayGetValueAtIndex(power_sources, 0));
    if (description) {
        int current_capacity = 0;
        int max_capacity = 0;
        char power_state[64] = {0};

        // data fields
        CFNumberRef current_capacity_num = CFDictionaryGetValue(description, CFSTR(kIOPSCurrentCapacityKey));
        CFNumberRef max_capacity_num = CFDictionaryGetValue(description, CFSTR(kIOPSMaxCapacityKey));
        CFStringRef state_str = CFDictionaryGetValue(description, CFSTR(kIOPSPowerSourceStateKey));
        if (current_capacity_num) {
            CFNumberGetValue(current_capacity_num, kCFNumberIntType, &current_capacity);
        }
        if (max_capacity_num) {
            CFNumberGetValue(max_capacity_num, kCFNumberIntType, &max_capacity);
        }
        if (state_str) {
            CFStringGetCString(state_str, power_state, sizeof(power_state), kCFStringEncodingUTF8);
        }

        int percentage = (max_capacity > 0) ? (current_capacity * 100 / max_capacity) : 0;

        int is_charging = (strcmp(power_state, kIOPSACPowerValue) == 0);
        // printf("  %d CONNECTED?:-> %d ", percentage, is_charging);
        if (percentage >= 80 && is_charging) {
            if (!notification_sent) {
                enum langs {
                    ENGLISH = 0,
                    SPANISH = 1
                };
                int current_language = 1;
                switch (current_language) {
                case ENGLISH: system("osascript -e 'tell application \"System Events\" to display dialog \"(DESCONECTAR MAC)UNPLUG MAC: Battery has hit 80%!\" buttons {\"OK\"} default button \"OK\" with title \"Battery Alert\" with icon caution' &"); break;
                case SPANISH: system("osascript -e 'tell application \"System Events\" to display dialog \"DESCONECTAR MAC: Batería alcanzó 80%!\" buttons {\"OK\"} default button \"OK\" with title \"Alerta de batería\" with icon caution' &"); break;
                }

                notification_sent = 1;
            }
        } else if (!is_charging) {

            notification_sent = 0;
        } else if (percentage < 80) {

            notification_sent = 0;
        }
    }

    CFRelease(power_sources);
    CFRelease(power_info);
}

void power_source_changed_callback() {
    check_battery_and_notify();
}

int main(void) {
    printf("sentinela esta ACTIVO\n");

    check_battery_and_notify();
    CFRunLoopSourceRef loop_source = IOPSNotificationCreateRunLoopSource(
      power_source_changed_callback, NULL);
    if (!loop_source) {
        fprintf(stderr, "Failed to create IOKit power notification loop source.\n");

        return 1;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(), loop_source, kCFRunLoopDefaultMode);
    CFRelease(loop_source);

    CFRunLoopRun();

    printf("sentinela esta SHUTINGDOWN\n");
    return 0;
}
