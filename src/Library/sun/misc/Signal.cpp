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

#include "Signal.h"

JCALL void lib_sun_misc_Signal_findSignal(const NativeArgs& args)
{
    const Object* sigNameObject = args.getObject(0);
    const std::u16string_view sigName = VM::get()->getHeap()->getStringContent(sigNameObject);
    const i4 signal = -1; // TODO: Implement signals on supported OS's
    const u4 signalUnsigned = static_cast<u4>(signal);
    args.thread->returnVar(Variable{VariableType_INT, signalUnsigned});
    printf("");
}
