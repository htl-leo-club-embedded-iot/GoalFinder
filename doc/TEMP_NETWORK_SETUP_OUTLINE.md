# TEMP: Network Setup Documentation Outline

This temporary outline describes what should be added to the user documentation for:
- Manual external network setup (static IP)
- WPA/WPA2 enterprise external network setup

## 1. Manual Network Setup (Static IP)

### 1.1 Preconditions
- Device is reachable in AP mode.
- User can open the web settings page.
- External Wi-Fi SSID is known.
- Network administrator provided static IPv4 values.

### 1.2 UI Flow
- Open Settings -> Connection -> External network.
- Enable `Use external network`.
- Enter `External network SSID`.
- Select authentication mode:
  - `Open` or
  - `WPA/WPA2 Personal` and password.
- Disable `Use DHCP`.
- Enter:
  - IP address
  - Default gateway
  - Subnet mask
  - DNS server
- Click `Apply network configuration`.
- Confirm restart when prompted.

### 1.3 Validation Guidance
- Explain valid IPv4 format: `x.x.x.x`.
- Warn against duplicate IP addresses.
- Recommend testing reachability after reboot.

### 1.4 Failure Recovery
- If connection fails, device falls back to AP mode.
- Instruct user to reconnect to AP and re-check values.

## 2. WPA/WPA2 Enterprise Setup

### 2.1 Preconditions
- SSID and enterprise credentials are available.
- User knows required EAP/phase 2 method from network admin.
- If certificate-based setup is required, cert/key material is available.

### 2.2 UI Flow
- Open Settings -> Connection -> External network.
- Enable `Use external network`.
- Enter `External network SSID`.
- Set `Authentication mode` to `WPA/WPA2 Enterprise`.
- Fill enterprise fields as required:
  - Enterprise username
  - Enterprise password
  - Optional: identity
  - Optional: anonymous identity
  - Optional: phase 2 method (`Automatic`, `MSCHAPv2`, `GTC`)
  - Optional: CA certificate
  - Optional: client certificate
  - Optional: client private key
- Click `Apply network configuration`.
- Confirm restart when prompted.

### 2.3 Certificate Input Notes
- Certificates and private keys must be pasted as full PEM text.
- Client certificate and private key must be provided together.
- If only one of them is provided, connection setup is rejected.

### 2.4 Security Notes
- Password fields can be saved without being re-shown in plain text.
- Explain safe handling of copied key material.
- Recommend rotating credentials on shared devices.

## 3. Troubleshooting Additions

### 3.1 Connection Does Not Establish
- Verify SSID and credentials.
- Verify enterprise username/password.
- Try phase 2 `Automatic` first.
- Verify static network values if DHCP is disabled.

### 3.2 Device Reverted to AP Mode
- Explain this is expected safety fallback.
- Tell user to reconnect and correct configuration.

### 3.3 Enterprise Certificate Errors
- Recheck PEM formatting.
- Recheck certificate/key pair.
- Recheck CA certificate requirements with network admin.

## 4. Screenshot Checklist (to add later)
- External network section with auth mode selector.
- Manual static fields.
- Enterprise credential fields.
- Restart confirmation modal.
