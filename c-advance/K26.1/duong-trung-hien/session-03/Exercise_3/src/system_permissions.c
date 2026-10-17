#include "system_permissions.h"

bool has_permission(uint8_t user_perms, uint8_t required_perms)
{  
    if((user_perms & required_perms) == required_perms)
    {
        return true;
    }
    else
    {
        return false;
    }
}

const char *permission_to_string(uint8_t required_perms)
{
    switch(required_perms) 
    {
        case PERM_READ:
            return "Read";

        case PERM_WRITE:
            return "Write";
            
        case PERM_EXECUTE:
            return "Execute";

        case PERM_DELETE:
            return "Delete";

        case PERM_READ | PERM_WRITE:
            return "Read AND Write";

        case PERM_READ | PERM_EXECUTE:
            return "Read AND Execute";
            
        case PERM_READ | PERM_DELETE:
            return "Read AND Delete";
            
        case PERM_WRITE | PERM_EXECUTE:
            return "Write AND Execute";
            
        case PERM_WRITE | PERM_DELETE:
            return "Write AND Delete";
            
        case PERM_EXECUTE | PERM_DELETE:
            return "Execute AND Delete";

        default:
            return "Unknown!";
    }
}

void result_check(uint8_t user_perms, uint8_t required_perms)
{
    printf("Checking for %s permission... ", permission_to_string(required_perms));
    if (has_permission(user_perms, required_perms))
    {
        printf("GRANTED\n");
    }
    else
    {
        printf("DENIED\n");
    }
}

