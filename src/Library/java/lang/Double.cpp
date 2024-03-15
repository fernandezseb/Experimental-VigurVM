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

#include "Double.h"


JCALL void lib_java_lang_Double_doubleToRawLongBits(const NativeArgs& args)
{
    const Variable highByte = args.thread->m_currentFrame->localVariables[1];
    const Variable lowByte = args.thread->m_currentFrame->localVariables[0];
    args.vm->checkType(highByte, VariableType_DOUBLE, args.thread);
    args.vm->checkType(lowByte, VariableType_DOUBLE, args.thread);

    args.thread->returnVar(
        Variable{VariableType_LONG, highByte.data},
        Variable{VariableType_LONG, lowByte.data}
        );
}

JCALL void lib_java_lang_Double_longBitsToDouble(const NativeArgs& args) {
    const Variable highByte = args.thread->m_currentFrame->localVariables[1];
    const Variable lowByte = args.thread->m_currentFrame->localVariables[0];
    args.vm->checkType(highByte, VariableType_LONG, args.thread);
    args.vm->checkType(lowByte, VariableType_LONG, args.thread);

    args.thread->returnVar(
        Variable{VariableType_DOUBLE, highByte.data},
        Variable{VariableType_DOUBLE, lowByte.data}
        );
}