#include "business_processor.h"

BussinessProcessor::BussinessProcessor() {
MP_REGISTER_MESSAGE_PROCESSOR(BussinessProcessor,
    ((Order, processOrder))
    ((CancelOrder, processCancelOrder))
)        
}

template<int assemble, int mode, typename InputMessageType>
class PrevStageWorker : public PrevStageWorkerBasic<assemble, InputMessageType>
{
public:
};
template<int fanout, typename OutputMessageType>
class NextStageWorker
{
public:
};

#define ADK_MAX_PIPELINE_PARALLEL           4
#define ADK_MAX_OUTPUT_MESSAGE_TYPE         4

#define default_output_type(z, n, unused)   \
    typedef class {} DefaultOutputType##n;

BOOST_PP_REPEAT(ADK_MAX_OUTPUT_MESSAGE_TYPE, default_output_type, ~)

// #undef default_output_type



#define enum_output_type(z, n, unused)   \
    , int fanout##n = 1, typename OutputMessageType##n = DefaultOutputType##n

#define enum_base_class(z, n, unused)   \
    , public NextStageWorker<fanout##n, OutputMessageType##n>



template<int assemble, int mode, typename InputMessageType
        BOOST_PP_REPEAT(ADK_MAX_OUTPUT_MESSAGE_TYPE, enum_output_type, ~)
        >
class StageWorker : public PrevStageWorker<assemble, mode, InputMessageType>
                    BOOST_PP_REPEAT(ADK_MAX_OUTPUT_MESSAGE_TYPE, enum_base_class, ~)
{
public:
    typedef InputMessageType prev_type;

    #define enum_assert_fanout(z, n, unused) \
    if(fanout##n <= ADK_MAX_PIPELINE_PARALLEL);

    StageWorker()
    {
        assert(assemble <= ADK_MAX_PIPELINE_PARALLEL);
        BOOST_PP_REPEAT(ADK_MAX_OUTPUT_MESSAGE_TYPE, enum_assert_fanout, ~);
    }

    #define enum_forward_func(z, n, unused) \
    inline int32_t Forward(OutputMessageType##n& message, short dim, short idx)   \
    {   \
        return NextStageWorker<fanout##n, OutputMessageType##n>::DoForward(message, dim, idx); \
    }

    BOOST_PP_REPEAT(ADK_MAX_OUTPUT_MESSAGE_TYPE, enum_forward_func, ~)    
};