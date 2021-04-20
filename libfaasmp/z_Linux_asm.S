//  z_Linux_asm.S:  - microtasking routines specifically
//                    written for Intel platforms running Linux* OS
//// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
//// See https://llvm.org/LICENSE.txt for license information.
//// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Clang includes this symbol for ABI compatibility with libgomp
// Since it is preceeded by a `.` we have to include it in assembly
// We only include it to satify the linker and avoid to modify the codegen in 
// Clang, it is therefore only named and not used because we do not care about
// compatibility with any runtime

// Reduction
	.hidden	.gomp_critical_user_.reduction.var # @.gomp_critical_user_.reduction.var
	.type	.gomp_critical_user_.reduction.var,@object
	.section	.data..gomp_critical_user_.reduction.var,"",@
	.globl	.gomp_critical_user_.reduction.var
	.p2align	4
.gomp_critical_user_.reduction.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_.reduction.var, 8

// Atomic reduction
	.hidden	.gomp_critical_user_.atomic_reduction.var # @.gomp_critical_user_.atomic_reduction.var
	.type	.gomp_critical_user_.atomic_reduction.var,@object
	.section	.data..gomp_critical_user_.atomic_reduction.var,"",@
	.globl	.gomp_critical_user_.atomic_reduction.var
	.p2align	4
.gomp_critical_user_.atomic_reduction.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_.atomic_reduction.var, 8

// Critical section
	.hidden	.gomp_critical_user_.var # @.gomp_critical_user_.var
	.type	.gomp_critical_user_.var,@object
	.section	.data..gomp_critical_user_.var,"",@
	.globl	.gomp_critical_user_.var
	.p2align	4
.gomp_critical_user_.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_.var, 8

// Airport
	.hidden	.gomp_critical_user_airport.var # @.gomp_critical_user_airport.var
	.type	.gomp_critical_user_airport.var,@object
	.section	.data..gomp_critical_user_airport.var,"",@
	.globl	.gomp_critical_user_airport.var
	.p2align	4
.gomp_critical_user_airport.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_airport.var, 8

// Inf event
	.hidden	.gomp_critical_user_inf_event.var # @.gomp_critical_user_inf_event.var
	.type	.gomp_critical_user_inf_event.var,@object
	.section	.data..gomp_critical_user_inf_event.var,"",@
	.globl	.gomp_critical_user_inf_event.var
	.p2align	4
.gomp_critical_user_inf_event.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_inf_event.var, 8

// Closeplace
	.hidden	.gomp_critical_user_closeplace.var # @.gomp_critical_user_closeplace.var
	.type	.gomp_critical_user_closeplace.var,@object
	.section	.data..gomp_critical_user_closeplace.var,"",@
	.globl	.gomp_critical_user_closeplace.var
	.p2align	4
.gomp_critical_user_closeplace.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_closeplace.var, 8

// Starttreat
	.hidden	.gomp_critical_user_starttreat.var # @.gomp_critical_user_starttreat.var
	.type	.gomp_critical_user_starttreat.var,@object
	.section	.data..gomp_critical_user_starttreat.var,"",@
	.globl	.gomp_critical_user_starttreat.var
	.p2align	4
.gomp_critical_user_starttreat.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_starttreat.var, 8

// state_cumV
	.hidden	.gomp_critical_user_state_cumV.var # @.gomp_critical_user_state_cumV.var
	.type	.gomp_critical_user_state_cumV.var,@object
	.section	.data..gomp_critical_user_state_cumV.var,"",@
	.globl	.gomp_critical_user_state_cumV.var
	.p2align	4
.gomp_critical_user_state_cumV.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_state_cumV.var, 8

// openplace
	.hidden	.gomp_critical_user_openplace.var # @.gomp_critical_user_openplace.var
	.type	.gomp_critical_user_openplace.var,@object
	.section	.data..gomp_critical_user_openplace.var,"",@
	.globl	.gomp_critical_user_openplace.var
	.p2align	4
.gomp_critical_user_openplace.var:
	.int64	42                      # 0x2a
	.size	.gomp_critical_user_openplace.var, 8

