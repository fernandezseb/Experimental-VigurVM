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

#include "Core.h"
#include "ClassLoader/ClassLoader.h"
#include "Printer/ClassPrinter.h"
#include "ClassDumpConfig.h"

int main(int argc, char* argv[])
{
    Platform::initialize();
    if (argc > 1) {
        if (strncmp(argv[1], "-v", 2) == 0) {
            printf("ClassDump version %d.%d\n", CLASSDUMP_VERSION_MAJOR, CLASSDUMP_VERSION_MINOR);
        } else {
            Memory memory(1000, 5 * 1024 * 1024);
            const char* className = argv[1];

            ClassLoader classLoader;
            ClassPrinter classPrinter;
            const ClassInfo* classInfo = classLoader.readClass(className, &memory);
            classPrinter.printClass(*classInfo, &memory);
#ifndef NDEBUG
            memory.printSize();
#endif
        }
    } else {
        fprintf(stderr, "Error: Class name must be supplied\n");
        Platform::exitProgram(1);
    }
    Platform::cleanup();

    return 0;
}
