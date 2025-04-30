#include <mrpc/service/context.h>

namespace mrpc
{

thread_local static ContextPtrQueue* g_queue = nullptr;

ContextPtrQueue* GetCurrentThreadContextPtrQueue()
{
    return g_queue;
}

void SetCurrentThreadContextPtrQueue(ContextPtrQueue* queue)
{
    g_queue = queue;
}

}
