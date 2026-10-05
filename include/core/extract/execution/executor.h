#ifndef CORE_EXECUTOR_H
#define CORE_EXECUTOR_H

#include <vector>
#include <cstdint>
#include <string>

namespace extract {

/**
 * @brief Execution methods for shellcode
 */
enum class ExecutionMethod {
    CREATE_PROCESS,
    EARLY_BIRD,
    THREAD_HIJACK
};

/**
 * @brief Execute shellcode using specified method
 * @param shellcode Raw shellcode bytes
 * @param method Execution method to use
 * @return true if execution successful
 */
bool execute(const std::vector<uint8_t>& shellcode, ExecutionMethod method);

/**
 * @brief Execute shellcode from file
 * @param shellcode_path Path to shellcode file
 * @param method Execution method to use
 * @return true if execution successful
 */
bool execute_from_file(const std::string& shellcode_path, ExecutionMethod method);

} // namespace extract

#endif // CORE_EXECUTOR_H 