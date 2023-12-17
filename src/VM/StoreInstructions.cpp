#include "StoreInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

void astore_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable refVar = thread->currentFrame->popOperand();
    Variable* var =  &thread->currentFrame->localVariables[arg];
    var->data = refVar.data;
    var->type = refVar.type;
}
