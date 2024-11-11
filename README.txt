Windows debug api test

This is experiment to show minimum case to reproduce https://github.com/llvm/llvm-project/issues/67825 without lldb / llvm.

in detach_before_continue, we call DebugBreakProcess before ContinueDebugEvent for last EXCEPTION_DEBUG_EVENT event.
This is same behavior as LLDB and we can see ContinueDebugEvent after DebugBreakProcess would return error, and target process like Unity crash.

in continue_before_detach, we call ContinueDebugEvent for last EXCEPTION_DEBUG_EVENT event before DebugBreakProcess.
ContinueDebugEvent will not return error and Unity would not crash with this way.
