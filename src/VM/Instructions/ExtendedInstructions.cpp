/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "ExtendedInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

// TODO: Move to VM
static i2 readShort(VMThread* thread)
{
    const u1* code = thread->m_currentMethod->code->code;
    const u1 buffer[2] = {code[thread->m_pc++], code[thread->m_pc++]};
    const i2 value = static_cast<i2>(buffer[1]) | static_cast<i2>(buffer[0]) << 8;
    return value;
}

void ifnull(const InstructionInput& input)
{
    const i2 branchByte = readShort(input.thread);
    // uint8_t byte = thread->currentMethod->code->code[thread->pc-3+branchByte];
    const Variable ref = input.thread->m_currentFrame->popOperand();
    if (ref.data == 0) {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifnonnull(const InstructionInput& input)
{
    const i2 branchByte = readShort(input.thread);
    // uint8_t byte = thread->currentMethod->code->code[thread->pc-3+branchByte];
    const Variable ref = input.thread->m_currentFrame->popOperand();
    if (ref.data != 0) {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}
