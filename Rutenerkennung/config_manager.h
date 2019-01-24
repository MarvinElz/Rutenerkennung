#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

struct SBD_Config{
    bool filterByArea;
    bool filterByCircularity;
    bool filterByConvexity;
    bool filterByInertia;
    float maxArea;
    float maxCircularity;
    float maxConvexity;
    float maxInertiaRatio;
    float maxThreshold;
    float minArea;
    float minCircularity;
    float minConvexity;
    float minDistBetweenBlobs;
    float minInertiaRatio;
    size_t minRepeatability;
    float minThreshold;
    float thresholdStep;
};

#endif // CONFIG_MANAGER_H
