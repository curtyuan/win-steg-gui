# JPEG Steganography Implementation Notes

## Format Specification
- Implementation targets baseline JPEG (JFIF) format
- Using JPEG library from package manager for JPEG handling

## Current Limitations
- Supports only baseline JPEG format
- No streaming support for large files
- No specific file size limits implemented
- No specific JPEG quality requirements
- Tested primarily with reference image (lena.jpg)

## Error Handling
Basic error handling implemented for:
- Memory allocation failures (print error message)
- Invalid JPEG format detection
- Insufficient capacity for payload
- Basic DCT coefficient validation

## Implementation Focus
- Primary goal: Successful embedding and extraction of shellcode payload
- Byte-by-byte verification of extracted payload against original 