.intel_syntax

.global _access_random_place_to_place_memory_dep

.section .data

.section .text

/*
Access memory randomly, by pointer chasing,
each stride depends on the last read
*/
// (rdi, rsi, rdx) -> 
// (rax) pointer to buffer
// (rsi) size of buffer (power of 2)
// (rdx) iterations
// no need to pad stack since no function call
_access_random_place_to_place_memory_dep:
    mov rbx, [rdi]
    xor rcx, rcx
// will use as AND mask
    dec rsi 
beginz:
    cmp rdx, rcx
    jbe endz
    inc rcx
    and rbx, rsi
    lea rbx, [rdi + rbx]
    mov r8, [rbx]
    add rbx, r8
    jmp beginz
endz:
    mov rax, rbx
    ret
