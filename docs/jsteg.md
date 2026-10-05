## Current Task
we are heading to test jsteg as new algoritm for both steg and extract under core/ of both include and src

- file organization:
  for jsteg extract --> "include/core/jpeg_extractor[.cpp and .h]"
  for jsteg embeding --> "src/core/steg/jsteg[.cpp and .h]"


### Reference for current task:

there is a go repo for jsteg situated under
  "0_Refer-code/imageOperation/jsteg-master"



## afterward

after test finished keep next
1. uses same logic to execute a shellcode in raw binary format. for perform the comparison. as this is a security research on this realm.
2. we will put aes_data.bin into .rsrc section.
3. Obfuscator Plan A: using Obfuscator-llvm with minimum configuration to avoid large compiled result.
  Obfuscator Plan B repository -> "\0_Refer-code\maldev\Obfuscator-master" => this will be used after function after current test finished.