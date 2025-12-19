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
    // printf("[Running initIDs from FileOutputStream]\n");
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
    const Object* descriptorObject = thisObject->getObject(0);
    const i8 handle = descriptorObject->getLong(1);

    // TODO: Implement correct handle determination
    if (handle == 1)
    {
        const Array* array = args.getArray(1);
        const i4 offset = args.getInt(2);
        const i4 length = args.getInt(3);
        const i4 appendBoolean = args.getInt(4);
        if (appendBoolean != 0)
        {
            args.thread->internalError("Not implemented yet to append in FileOutputStreams", ErrorCode::NOT_IMPLEMENTED_YET);

        }
        Platform::print(&(((const char*)array->data)[offset]), length);
    } else
    {
        args.thread->internalError("Not implemented yet for arbitrary file handles", ErrorCode::NOT_IMPLEMENTED_YET);
    }


    printf("");
    // write the bytes to the file
}
