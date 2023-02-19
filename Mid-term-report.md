## MP.1 Data Buffer

Implemented a Ring Buffer backed by a `std::vector`. The Ring Buffer is initialized with a fixed max capacity, after
which old elements are replaced by new elements. A simple iterator interface is exposed that is just enough to enable
the `.end()` based API.
I could've used vector::erase, but technically that is linear in the number of items in the vector as it has to destruct
the removed item and then move the others down. This would have worked fine for the case here of 8 or so images.
For this purpose, implementing a proper ring was just an excuse to do the exercise as well as implement a custom
iterator which I had never done before.


## MP.2 Detector notes

FAST is actually really fast! getting <1ms on my computer
Harris and Shi-Tomasi are ~8-9ms.
BRISK is even slower at 25-26ms with a threshold of 90, so not very brisk.
ORB takes 4ms with default parameters.
AKAZE is 43ms with default parameters.