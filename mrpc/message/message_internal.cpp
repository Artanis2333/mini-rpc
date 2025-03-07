#include <mrpc/message/message_internal.h>

namespace mrpc
{

bool ParseSkipUnknown(uint32_t type, const uint8_t*& begin, const uint8_t* const end)
{
    switch (type)
    {
        case WIRETYPE_VARINT:
        {
            uint64_t host = 0;
            if (!Parse<TYPE_VAR_UINT64>(host, begin, end)) return false;
            break;
        }
        case WIRETYPE_FIXED64:
        {
            if (begin + sizeof(uint64_t) > end) return false;
            begin += sizeof(uint64_t);
            break;
        }
        case WIRETYPE_LENGTH_DELIMITED:
        {
            uint32_t size = 0;
            if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;
            if (begin + size > end) return false;
            begin += size;
            break;
        }
        case WIRETYPE_FIXED32:
        {
            if (begin + sizeof(uint32_t) > end) return false;
            begin += sizeof(uint32_t);
            break;
        }
        default:
        {
            return false;
            break;
        }
    }
    return true;
}

}
