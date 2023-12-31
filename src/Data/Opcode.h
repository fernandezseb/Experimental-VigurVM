#pragma once

#include "Core.h"

enum Opcode : uint8_t {
	// CONSTANTS
	i_nop = 0x00,
	i_aconst_null = 0x01,
	i_iconst_m1 = 0x02,
	i_iconst_0 = 0x03,
	i_iconst_1 = 0x04,
	i_iconst_2 = 0x05,
	i_iconst_3 = 0x06,
	i_iconst_4 = 0x07,
	i_iconst_5 = 0x08,
	i_lconst_0 = 0x09,
	i_lconst_1 = 0x0a,
	i_fconst_0 = 0x0b,
	i_fconst_1 = 0x0c,
	i_fconst_2 = 0x0d,
	i_dconst_0 = 0x0e,
	i_dconst_1 = 0x0f,
	i_bipush = 0x10,
	i_sipush = 0x11,
	i_ldc = 0x12,
	i_ldc_w = 0x13,
	i_ldc2_w = 0x14,
	// LOADS
	i_iload = 0x15,
	i_lload = 0x16,
	i_fload = 0x17,
	i_dload = 0x18,
	i_aload = 0x19,
	i_iload_0 = 0x1a,
	i_iload_1 = 0x1b,
	i_iload_2 = 0x1c,
	i_iload_3 = 0x1d,
	i_lload_0 = 0x1e,
	i_lload_1 = 0x1f,
	i_lload_2 = 0x20,
	i_lload_3 = 0x21,
	i_fload_0 = 0x22,
	i_fload_1 = 0x23,
	i_fload_2 = 0x24,
	i_fload_3 = 0x25,
	i_dload_0 = 0x26,
	i_dload_1 = 0x27,
	i_dload_2 = 0x28,
	i_dload_3 = 0x29,
	i_aload_0 = 0x2a,
	i_aload_1 = 0x2b,
	i_aload_2 = 0x2c,
	i_aload_3 = 0x2d,
	i_iaload = 0x2e,
	i_laload = 0x2f,
	i_faload = 0x30,
	i_daload = 0x31,
	i_aaload = 0x32,
	i_baload = 0x33,
	i_caload = 0x34,
	i_saload = 0x35,
	// STORES
	i_istore = 0x36,
	i_lstore = 0x37,
	i_fstore = 0x38,
	i_dstore = 0x39,
	i_astore = 0x3a,
	i_istore_0 = 0x3b,
	i_istore_1 = 0x3c,
	i_istore_2 = 0x3d,
	i_istore_3 = 0x3e,
	i_lstore_0 = 0x3f,
	i_lstore_1 = 0x40,
	i_lstore_2 = 0x41,
	i_lstore_3 = 0x42,
	i_fstore_0 = 0x43,
	i_fstore_1 = 0x44,
	i_fstore_2 = 0x45,
	i_fstore_3 = 0x46,
	i_dstore_0 = 0x47,
	i_dstore_1 = 0x48,
	i_dstore_2 = 0x49,
	i_dstore_3 = 0x4a,
	i_astore_0 = 0x4b,
	i_astore_1 = 0x4c,
	i_astore_2 = 0x4d,
	i_astore_3 = 0x4e,
	i_iastore = 0x4f,
	i_lastore = 0x50,
	i_fastore = 0x51,
	i_dastore = 0x52,
	i_aastore = 0x53,
	i_bastore = 0x54,
	i_castore = 0x55,
	i_sastore = 0x56,
	// STACK
	i_pop = 0x57,
	i_pop2 = 0x58,
	i_dup = 0x59,
	i_dup_x1 = 0x5a,
	i_dup_x2 = 0x5b,
	i_dup2 = 0x5c,
	i_dup2_x1 = 0x5d,
	i_dup2_x2 = 0x5e,
	i_swap = 0x5f,
	// MATH
	i_iadd = 0x60,
	i_ladd = 0x61,
	i_fadd = 0x62,
	i_dadd = 0x63,
	i_isub = 0x64,
	i_lsub = 0x65,
	i_fsub = 0x66,
	i_dsub = 0x67,
	i_imul = 0x68,
	i_lmul = 0x69,
	i_fmul = 0x6a,
	i_dmul = 0x6b,
	i_idiv = 0x6c,
	i_ldiv = 0x6d,
	i_fdiv = 0x6e,
	i_ddiv = 0x6f,
	i_irem = 0x70,
	i_lrem = 0x71,
	i_frem = 0x72,
	i_drem = 0x73,
	i_ineg = 0x74,
	i_lneg = 0x75,
	i_fneg = 0x76,
	i_dneg = 0x77,
	i_ishl = 0x78,
	i_lshl = 0x79,
	i_ishr = 0x7a,
	i_lshr = 0x7b,
	i_iushr = 0x7c,
	i_lushr = 0x7d,
	i_iand = 0x7e,
	i_land = 0x7f,
	i_ior = 0x80,
	i_lor = 0x81,
	i_ixor = 0x82,
	i_lxor = 0x83,
	i_iinc = 0x84,
	// CONVERSIONS
	i_i2l = 0x85,
	i_i2f = 0x86,
	i_i2d = 0x87,
	i_l2i = 0x88,
	i_l2f = 0x89,
	i_l2d = 0x8a,
	i_f2i = 0x8b,
	i_f2l = 0x8c,
	i_f2d = 0x8d,
	i_d2i = 0x8e,
	i_d2l = 0x8f,
	i_d2f = 0x90,
	i_i2b = 0x91,
	i_i2c = 0x92,
	i_i2s = 0x93,
	i_lcmp = 0x94,
	i_fcmpl = 0x95,
	i_fcmpg = 0x96,
	i_dcmpl = 0x97,
	i_dcmpg = 0x98,
	// CONTROL
	i_ifeq = 0x99,
	i_ifne = 0x9a,
	i_iflt = 0x9b,
	i_ifge = 0x9c,
	i_ifgt = 0x9d,
	i_ifle = 0x9e,
	i_if_icmpeq = 0x9f,
	i_if_icmpne = 0xa0,
	i_if_icmplt = 0xa1,
	i_if_icmpge = 0xa2,
	i_if_icmpgt = 0xa3,
	i_if_icmple = 0xa4,
	i_if_acmpeq = 0xa5,
	i_if_acmpne = 0xa6,
	i_goto = 0xa7,
	i_jsr = 0xa8,
	i_ret = 0xa9,
	i_tableswitch = 0xaa,
	i_lookupswitch = 0xab,
	i_ireturn = 0xac,
	i_lreturn = 0xad,
	i_freturn = 0xae,
	i_dreturn = 0xaf,
	i_areturn = 0xb0,
	i_return = 0xb1,
	// REFERENCES
	i_getstatic = 0xb2,
	i_putstatic = 0xb3,
	i_getfield = 0xb4,
	i_putfield = 0xb5,
	i_invokevirtual = 0xb6,
	i_invokespecial = 0xb7,
	i_invokestatic = 0xb8,
	i_invokeinterface = 0xb9,
	i_invokedynamic = 0xba,
	i_new = 0xbb,
	i_newarray = 0xbc,
	i_anewarray = 0xbd,
	i_arraylength = 0xbe,
	i_athrow = 0xbf,
	i_checkcast = 0xc0,
	i_instanceof = 0xc1,
	i_monitorenter = 0xc2,
	i_monitorexit = 0xc3,
	// EXTENDED
	i_wide = 0xc4,
	i_multianewarray = 0xc5,
	i_ifnull = 0xc6,
	i_ifnonnull = 0xc7,
	i_goto_w = 0xc8,
	i_jsr_w = 0xc9,
	// RESERVED
	i_breakpoint = 0xca,
	i_impdep1 = 0xfe,
	i_impdep2 = 0xff
};