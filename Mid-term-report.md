## MP.1 Data Buffer

Implemented a Ring Buffer backed by a `std::vector`. The Ring Buffer is initialized with a fixed max capacity, after
which old elements are replaced by new elements. A simple iterator interface is exposed that is just enough to enable
the `.end()` based API.
I could've used vector::erase, but technically that is linear in the number of items in the vector as it has to destruct
the removed item and then move the others down. This would have worked fine for the case here of 8 or so images.
For this purpose, implementing a proper ring was just an excuse to do the exercise as well as implement a custom
iterator which I had never done before.


## MP.2 Detector notes

- FAST is actually really fast! getting <1ms on my computer
- Harris and Shi-Tomasi are ~8-9ms.
- BRISK is even slower at 25-26ms with a threshold of 90, so not very brisk.
- ORB takes 4ms with default parameters.
- AKAZE is 43ms with default parameters.
- SIFT is 48ms.

Obviously I need to write a more accurate benchmark setup for this.

## MP.3 Region of interest

This was really easy by using OpenCV's keypoint filtering abilities, masking the area of the rectangle.


## MP.4 Descriptor Extraction
## MP.5 Addition of FLANN and kNN
## MP.6 Addition of distance ratio test

## MP.7 Number of keypoints within the vehicle bounding box for each detector.

| Detector  |Median|p90|Image 1|Image 2|Image 3|Image 4|Image 5|Image 6|Image 7|Image 8|Image 9|Image 10|
|-----------|------|---|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
| SHITOMASI |119|123.2|125|118|123|120|120|113|114|123|111|112|
| AKAZE     |165|177.2|166|157|161|155|163|164|173|175|177|179|
| BRISK     |92.5|96.3|87|88|81|92|93|94|95|96|99|90|
| FAST      |62.5|66.3|63|57|62|60|59|65|58|63|69|66|
| HARRIS    |23.5|34.9|17|14|18|21|26|43|18|31|26|34|
| ORB       |119|129.1|92|102|106|113|109|125|130|129|127|128|
| SIFT      |137|149.1|138|132|124|137|134|140|137|148|159|137|

## MP.8 Number of matched keypoints

**For this exercise, the filter on only the preceding vehicle has been removed. The entire image is considered.**

| Detector  | Descriptor | median number of Matching keypoints | p90  |
|-----------|------------|-------------------------------------|------|
| AKAZE     | AKAZE      | 1025                                | 1039 |
| AKAZE     | BRIEF      | 985                                 | 1005 |
| AKAZE     | FREAK      | 870                                 | 904  |
| AKAZE     | ORB        | 878                                 | 905  |
| AKAZE     | SIFT       | 1055                                | 1058 |
| BRISK     | AKAZE      | n/a                                 | n/a  |
| BRISK     | BRIEF      | 483                                 | 503  |
| BRISK     | FREAK      | 396                                 | 410  |
| BRISK     | ORB        | 420                                 | 435  |
| BRISK     | SIFT       | 483                                 | 504  |
| FAST      | AKAZE      | n/a                                 | n/a  |
| FAST      | BRIEF      | 438                                 | 446  |
| FAST      | FREAK      | 314                                 | 320  |
| FAST      | ORB        | 408                                 | 418  |
| FAST      | SIFT       | 471                                 | 487  |
| HARRIS    | AKAZE      | n/a                                 | n/a  |
| HARRIS    | BRIEF      | 93                                  | 143  |
| HARRIS    | FREAK      | 77                                  | 116  |
| HARRIS    | ORB        | 89                                  | 132  |
| HARRIS    | SIFT       | 95                                  | 154  |
| ORB       | AKAZE      | n/a                                 | n/a  |
| ORB       | BRIEF      | 246                                 | 251  |
| ORB       | FREAK      | 127                                 | 131  |
| ORB       | ORB        | 336                                 | 342  |
| ORB       | SIFT       | 371                                 | 377  |
| SHITOMASI | AKAZE      | n/a                                 | n/a  |
| SHITOMASI | BRIEF      | 981                                 | 1003 |
| SHITOMASI | FREAK      | 714                                 | 724  |
| SHITOMASI | ORB        | 890                                 | 908  |
| SHITOMASI | SIFT       | 1045                                | 1068 |
| SIFT      | AKAZE      | n/a                                 | n/a  |
| SIFT      | BRIEF      | 653                                 | 673  |
| SIFT      | FREAK      | 522                                 | 538  |
| SIFT      | ORB        | n/a                                 | n/a  |
| SIFT      | SIFT       | 759                                 | 815  |

## MP.9

Spreadsheet https://docs.google.com/spreadsheets/d/1E16v0HlMi7SVAiLAv7BoYoxCEssw3J2XNfaT_DTUSdo/edit#gid=557925971

Top 3:
FAST + BRIEF
FAST + ORB
Maybe SIFT + SIFT

FAST + BRIEF is the fastest combination. In our goal of tracking only the preceding vehicle, rotation invariance is not as critical to the goal as most of the time the car in front of us will maintain its orientation across frames. So, to minimize latency, I would choose this combo.
FAST + ORB is the 2nd faster. In addition in case of turns or due to bumps in the road, the preceding vehicle can experience some rotation, in which case ORB extraction provides rotation invariance. Since it uses pyramids for some multi-scale capability, this will also accomodate for slight changes in distance between the two vehicles, which is another point in favor of FAST+ORB.
Note that ORB is a combination of FAST for detection and BRIEF for extraction, with improvements to each, so in my opinion, even though FAST + BRIEF is third fastest, it doesn't make sense to consider it a top 3, as ORB theoretically has better matching.
Some cursory literature reading suggests that SIFT continues to be more robust to brightness and other changes compared to the other algorithms. So even though it takes the most time, if we are OK with only processing 10 images every second (since SIFT was taking 77ms on my machine -> 1000ms/(77ms rounded to 100) = 10), then that robustness may be worth it.