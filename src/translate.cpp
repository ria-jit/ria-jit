//
// Created by flo on 09.05.20.
//

#include "translate.h"
#include <iostream>
#include <asmjit/asmjit.h>

using namespace asmjit;

void test_simple_assembly();
void test_strlen_example();

/**
 * Just a quick example to test code generation using AsmJit.
 * Externed to be called from C code.
 */
void test_generation() {
    std::cout << "Testing code generation with a few examples...\n";

    test_simple_assembly();
    test_strlen_example();
}

void test_strlen_example() {//experiment with a strlen example
    JitRuntime runtime;
    CodeHolder codeHolder;

    codeHolder.init(runtime.codeInfo());
    x86::Assembler a(&codeHolder);

    //create the labels for our strlen call
    Label loop_cond = a.newLabel();
    Label loop_break = a.newLabel();

    typedef int (*asm_strlen)(char*);

    //unix calling convention, so the first argument is in rdi, return value in rax
    x86::Gp str = x86::rdi;
    x86::Gp len = x86::rax;

    a.xor_(len, len);

    a.bind(loop_cond);
    a.cmp(x86::byte_ptr(str), 0);
    a.jz(loop_break);
    a.inc(len);
    a.inc(str);
    a.jmp(loop_cond);

    a.bind(loop_break);
    a.ret();

    asm_strlen generated;
    Error err = runtime.add(&generated, &codeHolder);
    if (err) {
        std::cout << "Bad. Error in assembly. Code: " << std::dec << err;
        return;
    }

    std::string hello = "DynamicBinaryTranslation";
    char *string = const_cast<char *>(hello.c_str());
    int result = generated(string); //expected 24
    std::cout << "Length of string " << hello << " is " << std::dec << result;

    runtime.release(generated);
}

void test_simple_assembly() {
    JitRuntime runtime;
    CodeHolder codeHolder;

    codeHolder.init(runtime.codeInfo());
    x86::Assembler assembler(&codeHolder);

    std::cout << "Emitting code mov eax, 0xBEEF\n";
    assembler.mov(x86::eax, 0xBEEF);
    assembler.ret();

    //main function to call the generated code experiment
    typedef int (*Exec)();
    Exec output;
    Error error = runtime.add(&output, &codeHolder);
    if (error) {
        std::cout << "Bad. Error occurred during code generation.\n";
        return;
    }

    int retValue = output();
    std::cout << "Function returned " << std::hex << retValue << ", expected was 0xBEEF :)\n";

    runtime.release(output);
}
