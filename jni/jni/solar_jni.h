#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// Configure the CRMP connection used by the native client.
// Call before starting the game/network layer.
void solar_configure_server(const char* ip, int port);

// Read currently configured values.
const char* solar_server_ip();
int solar_server_port();

#ifdef __cplusplus
}
#endif
