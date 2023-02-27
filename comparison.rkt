#lang racket
(require racket/list)
(require math/statistics)

(define workdir "/home/nikhil/SFND_2D_Feature_Tracking/cmake-build-debug")
(define exe "2D_feature_tracking")

(define detectors '("SHITOMASI" "HARRIS" "FAST" "BRISK" "ORB" "AKAZE" "SIFT"))
(define descriptors '("BRIEF" "ORB" "FREAK" "AKAZE" "SIFT"))
(for ([pair (in-list (cartesian-product detectors descriptors))])
  (match-define-values (_ _ wall-time _) (time-apply (lambda () (with-output-to-file (format "/tmp/test_results_~a_~a.txt" (first pair) (second pair)) #:exists 'replace
                                                                  (lambda ()
                                                                    (parameterize ([current-directory workdir])
                                                                      (match-define (list _ _ _ _ waiter)
                                                                        (process*/ports (current-output-port) #f 'stdout "/home/nikhil/SFND_2D_Feature_Tracking/cmake-build-debug/2D_feature_tracking"
                                                                                        (first pair) (second pair)
                                                                                        #:set-pwd? #t))
                                                                      (waiter 'wait))))) '()))
  (printf "DONE with ~v ~v in ~vms~n" (first pair) (second pair) wall-time))