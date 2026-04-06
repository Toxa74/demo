#ifndef DOCFRONTPROCESSOR_H
#define DOCFRONTPROCESSOR_H

#include <QImage>
#include <GlareDetector.h>
#include <BlurDetector.h>

#include "contour/ContourDetector.h"
#include "feature/FeatureDetector.h"
#include "Settings.h"

namespace doc {

class Processor
{
public:

    struct ProcResult {
        ContourDetector::Result contour;
        FeatureDetector::Result feature;
        float blur_mean;
        std::vector<QPolygon> glare;
    };

    Processor() = default;

    virtual void reset() =0;

    virtual void processFrame(const cv::Mat &src_frame,
                              std::function<void()>                             cb_on_start,
                              std::function<void(ProcResult)>                   cb_on_result,
                              std::function<void(ProcResult, float)>            cb_on_progress) = 0;


    static std::unique_ptr<Processor> create(const Settings::Doc &opts,
                                             std::unique_ptr<ContourDetector>        contour_detector,
                                             std::unique_ptr<FeatureDetector>        feature_detector,
                                             std::unique_ptr<blur::BlurDetector>     blur_detector,
                                             std::unique_ptr<glare::GlareDetector>   glare_detector,
                                             std::function<void (const QString &)>   cb_log);
};
}

#endif // DOCFRONTPROCESSOR_H
