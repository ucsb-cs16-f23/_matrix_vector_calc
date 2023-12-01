/*
*   IMPORTANT
*   icc may required for _asm block
*/

unsigned short _m_flag;
int err_code = 114;
int err_code_mul = 114;

extern "C" __declspec(dllexport) void __fastcall set_err_code(int _1, int  _2) {
    err_code = _1;
    err_code_mul = _2;
}

extern "C" __declspec(dllexport) void __fastcall _add_to(long long* a, long long b)
{
    __asm {
        mov rcx, a
        mov rdx, b
        add [rcx], rdx
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    };
    if (_m_flag)throw err_code;
}

extern "C" __declspec(dllexport) void __fastcall _sub_to(long long* a, long long b)
{
    __asm {
        mov rcx, a
        mov rdx, b
        sub [rcx], rdx
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    };
    if (_m_flag)throw err_code;
}

extern "C" __declspec(dllexport) void __fastcall _mul_to(long long* a, long long b)
{
    __asm {
        mov rcx, a
        mov rax, [rcx]
        mov rdx, b
        imul rdx
        mov [rcx], rax
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    };
    if (_m_flag)throw err_code_mul;
}

extern "C" __declspec(dllexport) unsigned short get_flags()
{
    unsigned short _ret;
    __asm {
        pushfw
        pop ax
        mov _ret, ax
    };
    return _ret;
}

extern "C" __declspec(dllexport) long long __fastcall safe_add64(long long a, long long b) {
    __asm {
        mov rdx, b
        add a, rdx
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    }
    if (_m_flag)throw err_code;
    return a;
}

extern "C" __declspec(dllexport) long long __fastcall safe_sub64(long long a, long long b) {
    __asm {
        mov rdx, b
        sub a, rdx
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    }
    if (_m_flag)throw err_code;
    return a;
}

extern "C" __declspec(dllexport) long long __fastcall safe_mul64(long long a, long long b) {
    __asm {
        mov rax, a
        mov rdx, b
        imul rdx
        mov a, rax
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    }
    if (_m_flag)throw err_code_mul;
    return a;
}

extern "C" __declspec(dllexport) short __fastcall safe_add16(short a, short b) {
    _asm {
        mov ax, a
        add ax, b
        mov a, ax
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag,ax
    }
    if(_m_flag)throw err_code;
    return a;
}

extern "C" __declspec(dllexport) short __fastcall safe_sub16(short a, short b) {
    _asm {
        mov dx,b
        sub a, dx
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    }
    if (_m_flag)throw err_code;
    return a;
}

extern "C" __declspec(dllexport) short __fastcall safe_mul16(short a, short b) {
    __asm {
        mov ax, a
        mov dx, b
        imul dx
        mov a, ax
        pushfw
        pop ax
        and ax, 2048
        mov _m_flag, ax
    }
    if (_m_flag)throw err_code_mul;
    return a;
}