#pragma once

// Single server endpoint used by the client.
// Change only these two values when moving the client to another server.
#define SAMP_SERVER_HOST "80.242.59.112"
#define SAMP_SERVER_PORT 4145

// Keep the original two-entry UI layout for compatibility with code that
// assumes server indexes 0 and 1 exist. Both entries intentionally point to
// the same endpoint.
#define SAMP_SERVER_NAME_0 "MY SERVER"
#define SAMP_SERVER_NAME_1 "MY SERVER"
