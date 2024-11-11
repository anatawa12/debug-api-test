#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void printLastError(const char *context) {
    DWORD lastError = GetLastError();

    LPVOID lpMsgBuf;
    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            lastError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

    fprintf(stderr, "error %s (%lu): %s\n", context, lastError, (char *)lpMsgBuf);
}

void printReceivedEvent(DWORD eventCode) {
    switch (eventCode) {
        default:
            printf("received unknown: %lu\n", eventCode);
            break;
        case EXCEPTION_DEBUG_EVENT:
            printf("received EXCEPTION_DEBUG_EVENT\n");
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            printf("received CREATE_THREAD_DEBUG_EVENT\n");
            break;
        case CREATE_PROCESS_DEBUG_EVENT:
            printf("received CREATE_PROCESS_DEBUG_EVENT\n");
            break;
        case EXIT_THREAD_DEBUG_EVENT:
            printf("received EXIT_THREAD_DEBUG_EVENT\n");
            break;
        case EXIT_PROCESS_DEBUG_EVENT:
            printf("received EXIT_PROCESS_DEBUG_EVENT\n");
            break;
        case LOAD_DLL_DEBUG_EVENT:
            printf("received LOAD_DLL_DEBUG_EVENT\n");
            break;
        case UNLOAD_DLL_DEBUG_EVENT:
            printf("received UNLOAD_DLL_DEBUG_EVENT\n");
            break;
        case OUTPUT_DEBUG_STRING_EVENT:
            printf("received OUTPUT_DEBUG_STRING_EVENT\n");
            break;
        case RIP_EVENT:
            printf("received RIP_EVENT\n");
            break;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "%s <pid>", argv[0]);
        return 1;
    }
    int pid = strtol(argv[1], NULL, 10);

    printf("attaching %d\n", pid);

    if (!DebugActiveProcess(pid)) {
        printLastError("attaching process");
        return 1;
    }

    DEBUG_EVENT dbe = {};
    CREATE_PROCESS_DEBUG_INFO processDebugInfo = {};

    while (1) {
        if (!WaitForDebugEvent(&dbe, 1000)) {
            if (GetLastError() == ERROR_SEM_TIMEOUT) {
                break;
            }
            printLastError("waiting for debug event");
            return 1;
        }
        printReceivedEvent(dbe.dwDebugEventCode);
        if (dbe.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
            processDebugInfo = dbe.u.CreateProcessInfo;
        }

        if (!ContinueDebugEvent(dbe.dwProcessId, dbe.dwThreadId, DBG_CONTINUE)) {
            printLastError("ContinueDebugEvent for create event");
        }
    }

    if (!DebugBreakProcess(processDebugInfo.hProcess)) {
        printLastError("DebugBreakProcess");
        return 1;
    }

    while (1) {
        if (!WaitForDebugEvent(&dbe, INFINITE)) {
            printLastError("waiting for debug event");
            return 1;
        }
        printReceivedEvent(dbe.dwDebugEventCode);
        if (dbe.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
            break;
        }

        if (!ContinueDebugEvent(dbe.dwProcessId, dbe.dwThreadId, DBG_CONTINUE)) {
            printLastError("ContinueDebugEvent for create event");
        }
    }

    if (!ContinueDebugEvent(dbe.dwProcessId, dbe.dwThreadId, DBG_CONTINUE)) {
        printLastError("ContinueDebugEvent");
    }

    DebugActiveProcessStop(pid);

    printf("detached\n");

    printf("End of program\n");
    return 0;
}
