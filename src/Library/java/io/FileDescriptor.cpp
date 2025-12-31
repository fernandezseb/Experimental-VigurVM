/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "FileDescriptor.h"

JCALL void lib_java_io_FileDescriptor_initIDs(const NativeArgs& args)
{
    // printf("[Running initIDs from FileDescriptor]\n");
}

JCALL void lib_java_io_FileDescriptor_set(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const vdata var = currentFrame->localVariables[0];

    u8 handle = 0u;
    switch (var.getInt())
    {
    case 0: // stdin
    case 1: // stdout
    case 2: // stderr
        handle = var.getInt();
        break;
    default:
        args.thread->internalError("Filedescriptor not found", 9);
    }

    args.thread->returnVar(vdata(VariableType_LONG, static_cast<vlong>(handle)));
}
