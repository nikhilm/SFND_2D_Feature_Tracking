## MP.1 Data Buffer

Implemented a Ring Buffer backed by a `std::vector`. The Ring Buffer is initialized with a fixed max capacity, after
which old elements are replaced by new elements. A simple iterator interface is exposed that is just enough to enable
the `.end()` based API.