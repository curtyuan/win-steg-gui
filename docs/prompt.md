- Please ignore linting error for this file.
# Win-GUI-Steg Current Development Focus

## Important Notes
- File structure please refer to "docs/file-structure.md" 
- If you think it's necessary to add a new file or organize function to specific file. please submit it.
    

## Previous Work
- Previous work has complete the test of extraction and decryption in "tests/extract_decrypt_test".


### Test Materials
- For stealth_execute_test:
  - assets/images/lena3.bmp (encrypted shellcode in image)
  - assets/bins/aes_data.bin (key/IV for decryption)
- For early_map_test:
  - assets/bins/revshell_plaintext.bin (raw shellcode)
- For image steg and extract test:
  - original picture: "assets/images/lena.jpg"

### Test Organization
all test are organized under tests/, test files are independent. shouldn't include other test file.
- for current task we test in "tests/jsteg_test"

# Current Task
our current task is keep finish stealth_execute_test. 
relevant requirement is written in "docs/stealth-design.md"




