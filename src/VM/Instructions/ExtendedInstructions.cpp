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

static u2 readShort(VMThread* thread)
{
    uint8_t indexByte1 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint8_t indexByte2 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}

void ifnull(INSTRUCTION_ARGS)
{
    const u2 branchByte = readShort(thread);
    // uint8_t byte = thread->currentMethod->code->code[thread->pc-3+branchByte];
    const Variable ref = thread->m_currentFrame->popOperand();
    if (ref.data == 0) {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifnonnull(INSTRUCTION_ARGS)
{
    const u2 branchByte = readShort(thread);
    // uint8_t byte = thread->currentMethod->code->code[thread->pc-3+branchByte];
    const Variable ref = thread->m_currentFrame->popOperand();
    if (ref.data != 0) {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}
