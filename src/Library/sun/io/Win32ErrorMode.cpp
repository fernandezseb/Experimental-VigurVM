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

#include "Win32ErrorMode.h"
#include "Platform.h"

JCALL void lib_sun_io_Win32ErrorMode_setErrorMode(const NativeArgs& args)
{
    const vdata flags = args.thread->m_currentFrame->localVariables[0];
    const u4 errorModeValue = flags.getLong();

    Platform::WinAPISetErrorMode(errorModeValue);
    args.thread->returnVar(
        vdata{VariableType_LONG, flags.getLong()}
        );
}
