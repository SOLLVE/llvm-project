// RUN: %clang_cc1 -triple x86_64-pc-windows-msvc19.0.24215 -std=c99 -disable-legacy-loop-transformations -ast-print %s | FileCheck --check-prefix=PRINT --match-full-lines %s
// RUN: %clang_cc1 -triple x86_64-pc-windows-msvc19.0.24215 -std=c99 -disable-legacy-loop-transformations -emit-llvm -disable-llvm-passes -o - %s | FileCheck --check-prefix=IR --match-full-lines %s
// RUN: %clang_cc1 -triple x86_64-pc-windows-msvc19.0.24215 -std=c99 -disable-legacy-loop-transformations -emit-llvm -O3 -mllvm -polly -mllvm -polly-process-unprofitable -mllvm -polly-use-llvm-names -mllvm -debug-only=polly-ast -o /dev/null %s 2>&1 > /dev/null | FileCheck --check-prefix=AST %s
// RUN: %clang_cc1 -triple x86_64-pc-windows-msvc19.0.24215 -std=c99 -disable-legacy-loop-transformations -emit-llvm -O3 -mllvm -polly -mllvm -polly-process-unprofitable -mllvm -polly-use-llvm-names -o - %s | FileCheck --check-prefix=TRANS %s
// RUN: %clang -DMAIN -std=c99 -disable-legacy-loop-transformations -O3 -mllvm -polly -mllvm -polly-process-unprofitable %s -o %t_pragma_pack%exeext
// RUN: %t_pragma_pack%exeext | FileCheck --check-prefix=RESULT %s

__attribute__((noinline))
void pragma_unrolling(double C[const restrict static 8], double A[const restrict static 8]) {
  #pragma clang loop unrolling full
  for (int i = 0; i < 8; i += 1)
    C[i] = A[i] + i;
}

#ifdef MAIN
#include <stdio.h>
#include <string.h>
int main() {
  double C[8];
  double A[8];
  memset(A, 0, sizeof(A));
  A[1] = 42;
  pragma_unrolling(C,A);
  printf("(%0.0f)\n", C[1]);
  return 0;
}
#endif


// PRINT-LABEL: void pragma_unrolling(double C[const restrict static 8], double A[const restrict static 8]) __attribute__((noinline)) {
// PRINT-NEXT: #pragma clang loop unrolling full
// PRINT-NEXT:    for (int i = 0; i < 8; i += 1)
// PRINT-NEXT:      C[i] = A[i] + i;
// PRINT-NEXT: }


// IR-LABEL: define dso_local void @pragma_unrolling(double* noalias dereferenceable(64) %C, double* noalias dereferenceable(64) %A) #0 {
// IR:         br label %for.cond, !llvm.loop !2
//
// IR: !2 = distinct !{!2, !3, !4, !5}
// IR: !3 = !{!"llvm.loop.disable_nonforced"}
// IR: !4 = !{!"llvm.loop.unroll.enable", i1 true}
// IR: !5 = !{!"llvm.loop.unroll.full"}


// AST: if (1)
// AST:     {
// AST:       Stmt_for_body(0);
// AST:       Stmt_for_body(1);
// AST:       Stmt_for_body(2);
// AST:       Stmt_for_body(3);
// AST:       Stmt_for_body(4);
// AST:       Stmt_for_body(5);
// AST:       Stmt_for_body(6);
// AST:       Stmt_for_body(7);
// AST:     }
// AST: else
// AST:     {  /* original code */ }


// TRANS-LABEL: @pragma_unrolling
// TRANS: store double %p_add
// TRANS: store double %p_add
// TRANS: store double %p_add
// TRANS: store double %p_add
// TRANS: store double %p_add
// TRANS: store double %p_add
// TRANS: store double %p_add
// TRANS: store double %p_add


// RESULT: (43)
