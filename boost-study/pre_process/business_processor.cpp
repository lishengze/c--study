#include "business_processor.h"

BussinessProcessor::BussinessProcessor() {
MP_REGISTER_MESSAGE_PROCESSOR(BussinessProcessor,
    ((Order, processOrder))
    ((CancelOrder, processCancelOrder))
)        
}