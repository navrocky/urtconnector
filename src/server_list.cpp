#include "server_list.h"

server_list::server_list()
: state_(0)
{
}

void server_list::change_state()
{
    state_++;
}


