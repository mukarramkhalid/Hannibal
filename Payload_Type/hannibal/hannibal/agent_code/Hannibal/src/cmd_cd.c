#include "config.h"

#ifdef INCLUDE_CMD_CD

#include "hannibal_tasking.h"


SECTION_CODE void cmd_cd(TASK t)
{
    HANNIBAL_INSTANCE_PTR

    CMD_CD *cd = (CMD_CD *)t.cmd;
    LPCWSTR path = cd->path;

    DWORD fileAttr = hannibal_instance_ptr->Win32.GetFileAttributesW(path);
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        TASK response_t;
        LPCWSTR response_content = L"Does Not Exist";
        response_t.output = (LPCSTR)response_content;
        response_t.output_size = pic_strlenW(response_content)*sizeof(WCHAR) + 2;
        response_t.task_uuid = t.task_uuid;

        task_enqueue(hannibal_instance_ptr->tasks.tasks_response_queue, &response_t);

        hannibal_instance_ptr->Win32.VirtualFree(cd->path, 0, MEM_RELEASE);
        hannibal_instance_ptr->Win32.VirtualFree(t.cmd, 0, MEM_RELEASE);

        return; 
    }

    BOOL result = hannibal_instance_ptr->Win32.SetCurrentDirectoryW(path);

    LPCWSTR response_content;
    if(result){
        response_content = (WCHAR *)hannibal_instance_ptr->Win32.VirtualAlloc(NULL, MAX_PATH*sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);
        DWORD len = hannibal_instance_ptr->Win32.GetCurrentDirectoryW(MAX_PATH, response_content);
    } else {
        response_content = L"Fail";
    }


    TASK response_t;
    response_t.output = (LPCSTR)response_content;
    response_t.output_size = pic_strlenW(response_content)*sizeof(WCHAR) + 2;
    response_t.task_uuid = t.task_uuid;

    task_enqueue(hannibal_instance_ptr->tasks.tasks_response_queue, &response_t);

    hannibal_instance_ptr->Win32.VirtualFree(cd->path, 0, MEM_RELEASE);
    hannibal_instance_ptr->Win32.VirtualFree(t.cmd, 0, MEM_RELEASE);
    
}

#endif