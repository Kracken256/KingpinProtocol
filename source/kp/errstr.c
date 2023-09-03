#define __KINGPIN_BACKEND

#include <kp/errstr.h>
#include <kp/core.h>

void kp_errstr(kp_status err, char *buf, kp_size maxlen)
{
    if (maxlen == 0)
        return;

    buf[0] = '\0';

    switch ((u64)err)
    {
    case KP_SUCCESS:
        kp_strncpy(buf, "Success", maxlen);
        break;
    case KP_FAIL:
        kp_strncpy(buf, "Fail", maxlen);
        break;
    case KP_DEPENDENCY_MISSING:
        kp_strncpy(buf, "Dependency missing", maxlen);
        break;
    case KP_INVALID_ARGUMENT:
        kp_strncpy(buf, "Invalid argument", maxlen);
        break;
    case KP_INVALID_STATE:
        kp_strncpy(buf, "Invalid state", maxlen);
        break;
    case KP_MEMORY_ERROR:
        kp_strncpy(buf, "Memory error", maxlen);
        break;
    case KP_INTERNAL_ERROR:
        kp_strncpy(buf, "Internal error", maxlen);
        break;
    case KP_NOT_IMPLEMENTED:
        kp_strncpy(buf, "Not implemented", maxlen);
        break;
    case KP_NOT_FOUND:
        kp_strncpy(buf, "Not found", maxlen);
        break;
    case KP_ALREADY_EXISTS:
        kp_strncpy(buf, "Already exists", maxlen);
        break;
    case KP_OUT_OF_BOUNDS:
        kp_strncpy(buf, "Out of bounds", maxlen);
        break;
    case KP_NOT_INITIALIZED:
        kp_strncpy(buf, "Not initialized", maxlen);
        break;
    case KP_NOT_SUPPORTED:
        kp_strncpy(buf, "Not supported", maxlen);
        break;

    case KP_SESSION_MAGIC_MISMATCH:
        kp_strncpy(buf, "Session magic value mismatch", maxlen);
        break;
    case KP_SESSION_VERSION_MISMATCH:
        kp_strncpy(buf, "Session version mismatch", maxlen);
        break;
    case KP_SESSION_PEER_ID_MISMATCH:
        kp_strncpy(buf, "Session peer ID mismatch", maxlen);
        break;
    case KP_SESSION_PEER_NOT_ALLOWED:
        kp_strncpy(buf, "Session peer not allowed", maxlen);
        break;
    case KP_SESSION_MAC_MISMATCH:
        kp_strncpy(buf, "Session MAC mismatch", maxlen);
        break;
    case KP_SESSION_CLOSED:
        kp_strncpy(buf, "Session closed", maxlen);
        break;
    case KP_SESSION_READ_ERROR:
        kp_strncpy(buf, "Session read error", maxlen);
        break;
    case KP_SESSION_CRC32_MISMATCH:
        kp_strncpy(buf, "Session CRC32 mismatch", maxlen);
        break;
    case KP_SESSION_MSG_FINISH:
        kp_strncpy(buf, "Session message finish", maxlen);
        break;

    default:
        kp_strncpy(buf, "Unknown error", maxlen);
        break;
    }
}