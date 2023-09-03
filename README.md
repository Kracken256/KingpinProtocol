# KingpinProtocol

KingpinProtocol is a secure, lightweight, and versatile network protocol designed for covert communications with minimal overhead.

## Key Features
- **Security**: KingpinProtocol employs ECDH key exchange and ChaCha20-SHA256 encryption to ensure robust security and privacy for all communications.
- **Efficiency**: With a small packet size and minimal overhead, KingpinProtocol is optimized for efficiency.
- **Flexibility**: Its design flexibility makes it easy to implement in any programming language or environment.
- **Covert Communication**: KingpinProtocol excels in covert communications, featuring a compact packet size to minimize detection and the ability to seamlessly integrate with other protocols.
- **Optional Mutual Authentication**: Supporting optional mutual authentication, it enables both parties to verify each other's identities using a list of trusted public keys.
- **Perfect Forward Secrecy (Work in Progress)**: KingpinProtocol utilizes ECDH key exchange to negotiate sessions and continuously renegotiates new sessions within the tunnel using random ephemeral keys. This method ensures that even if session keys are compromised, attackers cannot decrypt the data.
- **Ultra-Small Library**: Designed to be extremely compact, the entire KingpinProtocol library is less than 70 KB in size.
- **Portability**: The library is dependency-free and engineered for portability across all systems, including embedded systems.

## Warning
Please note that this code is currently in rapid development and is not yet suitable for production use. Keep an eye out for future stable releases.

## Streamlined Handshake
As of now, KingpinProtocol's handshake process consumes only 78 bytes of data to establish a session. The client (initiator) sends 40 bytes of data to the server (responder), and the server responds with 38 bytes of data. No post-handshake confirmation is required; if key negotiation fails, the session is automatically terminated.

Here's an example of a handshake using the default settings:
```
Client Initialization: 50d3010000000000184aae1dcae87bc37eb9cd3ab7b239a6e61d5de788f27062d3da0505e47a5969
Server Response: 52d30000000055b3a47757f3f56a1039750506452ed3a1eadf86cc08f7c6e9a043dc6b76765c
```

## Modified Handshake for Enhanced Stealth
For those seeking to reduce the detection rate, it is possible to apply a mask to the handshake. This can involve using a DRBG with a constant seed to XOR the handshake data. Both communicating parties must use the same mask, and the mask should be applied to the handshake before sending the data. This approach can effectively evade most filters designed to detect the handshake, making it suitable for embedding in plain HTTP traffic with base64 content encoding to mimic web traffic.

## License
KingpinProtocol is distributed under the GPLv3 license. For more details, please refer to the `LICENSE.md` file.