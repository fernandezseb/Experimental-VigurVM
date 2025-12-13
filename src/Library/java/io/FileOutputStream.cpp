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

#include "FileOutputStream.h"
#include "Error.h"
#include "Platform.h"

JCALL void lib_java_io_FileOutputStream_initIDs(const NativeArgs& args)
{
    printf("[Running initIDs from FileOutputStream]\n");
}

// Method descriptor: ([BIIZ)V
//
//     /**
// * Writes a sub array as a sequence of bytes.
// * @param b the data to be written
// * @param off the start offset in the data
// * @param len the number of bytes that are written
// * @param append {@code true} to first advance the position to the
// *     end of file
// * @exception IOException If an I/O error has occurred.
// */
// private native void writeBytes(byte b[], int off, int len, boolean append)
JCALL void lib_java_io_FileOutputStream_writeBytes(const NativeArgs& args) {
    // Get the file handle
    const Object* thisObject = args.getThisObjectReference();
    const Object* descriptorObject = args.heap->getObject(thisObject->fields[0].data->data);
    u4 data1 = descriptorObject->fields[1].data->data;
    Variable var = descriptorObject->fields[1].data[1];

    // TODO: Implement correct handle determination
    if (var.data == 1)
    {
        const StackFrame* currentFrame = args.thread->m_currentFrame;
        const Variable dataArray = currentFrame->localVariables[1];
        args.vm->checkType(dataArray, VariableType_REFERENCE, args.thread);
        const Array* array = args.heap->getArray(dataArray.data);
        const Variable lengthVar = currentFrame->localVariables[3]; // must be int
        Platform::print((const char*)array->data, lengthVar.data);
    } else
    {
        args.thread->internalError("Not implemented yet for arbitrary file handles", ErrorCode::NOT_IMPLEMENTED_YET);
    }


    printf("");
    // write the bytes to the file
}
