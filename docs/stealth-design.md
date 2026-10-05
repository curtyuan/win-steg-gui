## Previous Work
- Current decryption function tested successfully
- Key and IV for decryption located in "assets/bins/aes_data.bin"
- Completed testing of AES decryption and BMP LSB steganography - do not modify these functions

## Current Task

### Test Materials
- For stealth_execute_test:
  - assets/images/lena3.bmp (encrypted shellcode in image)
  - assets/bins/aes_data.bin (key/IV for decryption)
- For early_map_test:
  - assets/bins/revshell_plaintext.bin (raw shellcode)

### Test Organization

-  stealth_execute_test (Split APC and Thread):
   - Core Concept:
     - APC handles preparation
     - Error thread handles execution
     - Coordinated two-phase approach
   - Flow:
     a. Setup Phase:
        - Create suspended process
        - Queue APC that will:
          * Map key/IV data
          * Extract and decrypt shellcode
          * Prepare for execution
        - Keep process suspended until triggered
     b. GUI Phase:
        - Create GUI with two tabs
        - Wait for image loading error
     c. Execution Phase:
        - Image error occurs
        - Error thread:
          * Gets captured for later use
          * Triggers Early Bird to run APC
        - APC routine:
          * Maps key/IV data
          * Extracts and decrypts shellcode
          * Executes in captured error thread
        - Complete flow in APC


### Key Design Points:
- stealth_execute_test:
    1. we use normal apc with "CREATE_SUSPENDED" to hang the whole work at the beginning before gui initialize. this including local mapping aes_data.bin to memory for decryption
    2. when error loagin triggered. resume the apc. 
    3. apc keep finish rest of work in background. including hijack thread of error loading used to execute the decrypted shellcode.* APC handles complex preparation
  - workflow:
  1. create process of preparation work with our preparation function. not any outside executable.
  2. GUI starts
  3. thread of loading second image create
  4. Error handler triggered 
  5. Error handler resume preparation in apc which includeing extract, local mapping aes_data and decryption and hijace thread of original loading.
  6. using hijacked thread to execute the shellcode.

  so control flow will looks like:
create suspended process --> GUI --> loading image --> error triggered --> resume apc process --> hijack image loading thread to execute shellcode.

### Reference for current task:
- we are going to "use"(instead of implement it) both Early Bird APC injection and local mapping injection for shellcode execution.
  1. for refer of apc injection: "0_Refer-code\maldev\ApcInjection"
  2. for refer of early bird APC: "0_Refer-code/maldev/EarlyBird/EarlyBird"
  3. for refer of local mapping injection: "0_Refer-code/maldev/LocalMappingInjection"
  4. for refer of thread hijack:"0_Refer-code\maldev\LocalThreadHijacking"
  5. Documentation of these technique are three markdown file under "0_Refer-code\maldev\".

- Managing files locally in current project:
 I have copied them to 
"project_root/src/refer/exec/[early_bird.c and local_mapping_injection.c]"
- Don't include them directly, Instead, copy(and separate content) them into "project_root/[include, src]/refer/[early_bird.cpp local_mapping_injection.cpp and corresponding header .h file]"

## afterward

after test finished keep next
1. uses same logic to execute a shellcode in raw binary format. for perform the comparison. as this is a security research on this realm.
2. we will put aes_data.bin into .rsrc section.
3. Obfuscator Plan A: using Obfuscator-llvm with minimum configuration to avoid large compiled result.
  Obfuscator Plan B repository -> "\0_Refer-code\maldev\Obfuscator-master" => this will be used after function after current test finished.


# execution issue
1. Thread Creation Timing:
The thread is created when the second tab attempts to load a non-existent picture
This is separate from the main FLTK GUI thread
We don't need to preserve FLTK functionality in this thread since it's created specifically for our purpose
2. Thread Management:
We create the thread directly using Windows API when the second tab is created
This keeps it separate from FLTK's thread management
The thread is dedicated to our stealth execution
3. Callback Mechanism:
We can implement a callback system for the image loading thread
If the image exists, show it in the examiner
If not, we can use this thread for our stealth execution