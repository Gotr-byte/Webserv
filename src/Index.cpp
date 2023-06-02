#include "../includes/Index.hpp"

/*This index is used by the pollfd struct to fill the spaces that are not used.
Also you can determine if the array of pollfds is full and pass the ones that are
not full to a queue.*/
Index::Index(){
    taken = false;
}
Index::~Index(){}
