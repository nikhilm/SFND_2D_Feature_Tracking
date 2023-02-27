#include <numeric>
#include <opencv2/xfeatures2d.hpp>
#include "matching2D.hpp"

using namespace std;

// Find best matches for keypoints in two camera images based on several matching methods
void matchDescriptors(std::vector<cv::KeyPoint> &kPtsSource, std::vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource,
                      cv::Mat &descRef,
                      std::vector<cv::DMatch> &matches, std::string descriptorType, std::string matcherType,
                      std::string selectorType) {
    // configure matcher
    bool crossCheck = false;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0) {
        int normType = cv::NORM_HAMMING;
        if (descriptorType == "SIFT") {
            normType = cv::NORM_L2;
        }
        matcher = cv::BFMatcher::create(normType, crossCheck);
    } else if (matcherType.compare("MAT_FLANN") == 0) {
        if (descSource.type() != CV_32F)
        { // OpenCV bug workaround : convert binary descriptors to floating point due to a bug in current OpenCV implementation
            descSource.convertTo(descSource, CV_32F);
            descRef.convertTo(descRef, CV_32F);
        }

        matcher = cv::FlannBasedMatcher::create();
        cout << "FLANN matching";
    }

    if (descSource.empty() || descRef.empty()) {
        return;
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0) { // nearest neighbor (best match)

        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
    } else if (selectorType.compare("SEL_KNN") == 0) { // k nearest neighbors (k=2)
        const double minDescDistRatio = 0.8;
        std::vector<std::vector<cv::DMatch>> kMatches;
        double t = (double) cv::getTickCount();
        matcher->knnMatch(descSource, descRef, kMatches, /* k= */ 2);
        for (auto it = kMatches.cbegin(); it != kMatches.cend(); it++) {
            const auto bestMatches = *it;
            if (bestMatches.size() < 2) {
                matches.push_back(bestMatches[0]);
            } else {
                // i believe the .distance field is the distance between the feature
                // in the source image and the matched feature in the reference image.
                // this is the distance in the descriptor vector.
                // the smaller the distance, the more similar the descriptors are.
                // what kNN is giving us is, hey here are the 2 descriptors
                // in the reference image that were 1st and 2nd in "minimum SSD value".
                // if the best match is far better than the second match
                // then that is good, otherwise there may be ambiguity.
                // so that is what this tests.
                if (bestMatches[0].distance < minDescDistRatio * bestMatches[1].distance) {
                    matches.push_back(bestMatches[0]);
                }
            }
        }
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (kNN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
//        cout << "discard count " << (kMatches.size() - matches.size()) << "\n";
        // ...
    }
}

// Use one of several types of state-of-art descriptors to uniquely identify keypoints
void descKeypoints(vector<cv::KeyPoint> &keypoints, cv::Mat &img, cv::Mat &descriptors, string descriptorType) {
    // select appropriate descriptor
    cv::Ptr<cv::DescriptorExtractor> extractor;
    if (descriptorType.compare("BRISK") == 0) {

        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.

        extractor = cv::BRISK::create(threshold, octaves, patternScale);
    } else if (descriptorType == "BRIEF") {
        extractor = cv::xfeatures2d::BriefDescriptorExtractor::create();
    } else if (descriptorType == "ORB") {
        extractor = cv::ORB::create();
    } else if (descriptorType == "FREAK") {
        extractor = cv::xfeatures2d::FREAK::create();
    } else if (descriptorType == "AKAZE") {
        extractor = cv::AKAZE::create();
    } else if (descriptorType == "SIFT") {
        extractor = cv::SIFT::create();
    } else {
        std::cerr << "Unknown extractor type " << descriptorType << "\n";
        abort();
    }

    // perform feature description
    double t = (double) cv::getTickCount();
    extractor->compute(img, keypoints, descriptors);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
}

// Detect keypoints in image using the traditional Shi-Thomasi detector
void detKeypointsShiTomasi(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis) {
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double) cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, false, k);

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it) {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << "Shi-Tomasi detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis) {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Shi-Tomasi Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsHarris(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis) {
    constexpr int blockSize = 2;
    constexpr int apertureSize = 3;
    constexpr double k = 0.04;
    constexpr int minResponse = 100;
    constexpr double maxOverlap = 0;

    double t = (double) cv::getTickCount();
    cv::Mat dst = cv::Mat::zeros(img.size(), CV_32FC1);
    cv::cornerHarris(img, dst, blockSize, apertureSize, k);
    cv::Mat dst_norm, dst_norm_scaled;
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

    for (int i = 0; i < dst_norm.rows; ++i) {
        for (int j = 0; j < dst_norm.cols; ++j) {
            const int response = (int) dst_norm.at<float>(i, j);
            if (response > minResponse) {
                cv::KeyPoint newKeyPoint;
                newKeyPoint.pt = cv::Point2f(j, i);
                newKeyPoint.size = 2 * apertureSize;
                newKeyPoint.response = response;
                // perform non-maximum suppression (NMS) in local neighbourhood around new key point
                bool bOverlap = false;
                for (auto it = keypoints.begin(); it != keypoints.end(); ++it) {
                    double kptOverlap = cv::KeyPoint::overlap(newKeyPoint, *it);
                    if (kptOverlap > maxOverlap) {
                        // If it did overlap, choose the keypoint with higher response.
                        bOverlap = true;
                        if (newKeyPoint.response >
                            (*it).response) {                      // if overlap is >t AND response is higher for new kpt
                            *it = newKeyPoint; // replace old key point with new one
                            break;             // quit loop over keypoints
                        }
                    }
                }
                if (!bOverlap) {                                     // only add new key point if no overlap has been found in previous NMS
                    keypoints.push_back(newKeyPoint); // store new keypoint in dynamic list
                }
            }
        }
    }
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << "Harris detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis) {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Harris Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsFast(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis) {
    double t = (double) cv::getTickCount();
    cv::FAST(img, keypoints, 75, true, cv::FastFeatureDetector::TYPE_9_16);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << "FAST detection with n= " << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis) {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Harris Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsBrisk(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis) {
    double t = (double) cv::getTickCount();
    cv::Ptr<cv::BRISK> detector = cv::BRISK::create(90);
    detector->detect(img, keypoints);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << "BRISK detection with n= " << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis) {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "BRISK Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsORB(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis) {
    double t = (double) cv::getTickCount();
    cv::Ptr<cv::ORB> detector = cv::ORB::create();
    detector->detect(img, keypoints);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << "ORB detection with n= " << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis) {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "ORB Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsAKAZE(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis) {
    double t = (double) cv::getTickCount();
    cv::Ptr<cv::AKAZE> detector = cv::AKAZE::create();
    detector->detect(img, keypoints);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << "AKAZE detection with n= " << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis) {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "AKAZE Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsSIFT(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis) {
    double t = (double) cv::getTickCount();
    cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
    detector->detect(img, keypoints);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
//    cout << "SIFT detection with n= " << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis) {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "SIFT Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsModern(vector<cv::KeyPoint> &keypoints, cv::Mat &img, std::string detectorType, bool bVis) {
    if (detectorType == "FAST") {
        detKeypointsFast(keypoints, img, bVis);
    } else if (detectorType == "BRISK") {
        detKeypointsBrisk(keypoints, img, bVis);
    } else if (detectorType == "ORB") {
        detKeypointsORB(keypoints, img, bVis);
    } else if (detectorType == "AKAZE") {
        detKeypointsAKAZE(keypoints, img, bVis);
    } else if (detectorType == "SIFT") {
        detKeypointsSIFT(keypoints, img, bVis);
    } else {
        std::cerr << "Unknown detector " << detectorType << "\n";
        abort();
    }

}
