#include "docfrontprocessor.h"

#include <QElapsedTimer>
#include "utils.h"


using namespace doc;
using namespace std;


namespace  {


enum class ERotate {
    NO_ROTATE=0,
    ROT_180,
    END_OF_LIST
};

// Special behavior for ++ERotate
ERotate& operator++( ERotate &c );

// Special behavior for ERotate++
ERotate operator++( ERotate &c, int );


struct BlurResult {
    BlurResult(): mean{}{}
    float                       mean;
};

struct ThreadResult{
    ThreadResult():
            cropped_img{}, type{}, location{}, rotate{}, blur{}, glare{}, time_begin{} {}

    bool isEmpty() const{
        return time_begin.time_since_epoch().count() == 0;
    }

    bool isReadyOCR() const {
        if(time_begin.time_since_epoch().count() == 0)
            return false;

        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_begin).count()  >  ThreadResult::complete_time_ms)
            return true;

        return false;
    }

    cv::Mat                     cropped_img;
    FeatureDetector::Result     type;
    cntr_detect::Location       location;
    ERotate                     rotate;
    BlurResult                  blur;
    glare::GlareDetectorResult  glare;
    std::chrono::steady_clock::time_point time_begin;
    std::chrono::steady_clock::time_point time_update;
    static const int   complete_time_ms = 3000;
};



}







class DocFrontprocessorImpl : public doc::Processor {

private:
    DocFrontprocessorImpl(const Settings::Doc &opts,
                          std::unique_ptr<ContourDetector>      contour_detector,
                          std::unique_ptr<FeatureDetector>      feature_detector,
                          std::unique_ptr<blur::BlurDetector>   blur_detector,
                          std::unique_ptr<glare::GlareDetector> glare_detector,
                          std::function<void (const QString &)> cb_log):
        _opts(opts),
        _contour_detector(std::move(contour_detector)),
        _feature_detector(std::move(feature_detector)),
        _blur_detector(std::move(blur_detector)),
        _glare_detector(std::move(glare_detector)),

        _cb_log(cb_log),
        _proc_step(_proc_step_t::COLLECT){

    }

    void reset() override {
        _detectDuration.restart();
        _last_res = {};
        _proc_step = _proc_step_t::COLLECT;
    }



    ProcResult process(const cv::Mat &frame) {

        ProcResult res;

        //Find Contour
        _contour_detector->detect(frame);
        res.contour = _contour_detector->getResult();

        //Find glare
        std::vector<QPolygon> glare;
        if(_glare_detector && !res.contour._contour.isEmpty)
        {
            auto glare_result = _glare_detector->detect(frame, {} /*glare_debugger*/);
            if(glare_result) {

                const auto & loc = res.contour._contour;
                vector<cv::Point> location_points = {{(int) loc.leftTop.x,     (int) loc.leftTop.y},
                                                     {(int) loc.rightTop.x,    (int) loc.rightTop.y},
                                                     {(int) loc.rightBottom.x, (int) loc.rightBottom.y},
                                                     {(int) loc.leftBottom.x,  (int) loc.leftBottom.y}};
                _glare_detector->filterByContourLocation(glare_result->contours, location_points, 0.5);
                    for(const auto & contour : glare_result->contours){
                        QPolygon polygon;
                        for(const auto & p: contour.points){
                            polygon.push_back(QPoint(p.x, p.y));
                        }
                        if(!polygon.isEmpty())
                            glare.emplace_back(polygon);
                    }
            }
        }
        res.glare.swap(glare);

        //Find Blur
        if(_blur_detector && !res.contour._cropped_img.empty()){
            try {
                const auto & cropped_img = res.contour._cropped_img;
                auto blur_array = _blur_detector->calcFocusSegment(8, 8, cropped_img, img_proc::EImageFormat::BGR,
                                                                                cropped_img.cols, cropped_img.rows);
                res.blur_mean = static_cast<float>(accumulate(blur_array.begin(), blur_array.end(), 0.0f)) /
                                 static_cast<float>(blur_array.size());
                Log("Blur: " + QString::number(res.blur_mean));
            }
            catch (const runtime_error & e){
                Log("Blur Exception:" + QString::fromStdString(e.what()));
            }
        }

        //Find Type
        if(_feature_detector && !res.contour._cropped_img.empty()){
            const auto & cropped_img = res.contour._cropped_img;
            _feature_detector->detect(cropped_img);
            res.feature = _feature_detector->getResult();
        }

        if(!_opts.common.drawBorder){
            res.contour._contour ={};
        }
        return res;
    }

    void processFrame(const cv::Mat &frame,
                      std::function<void()>  cb_on_start,
                      std::function<void(ProcResult)>        cb_on_result,
                      std::function<void(ProcResult, float)> cb_on_progress) override {

        switch(_proc_step){
        case _proc_step_t::COLLECT:
        {
            const auto & res = process(frame);


            if(!_last_res.feature._is_valid){
                _detectDuration.restart();
            }

            if(cb_on_progress)
                cb_on_progress(res, _detectDuration.elapsed()/1000.0f);

            if(res.feature._is_valid){
                if(res.feature._confidence > _last_res.feature._confidence){
                    _last_res = res;
                }
            }

            if(_detectDuration.elapsed() > _opts.common.accumTimeMs && !_opts.common.nonStop){
                if(cb_on_result)
                   cb_on_result(_last_res);

                _showDuration.restart();
                _proc_step = _proc_step_t::SHOW_RESULT;
                Log("Start ShowResult");
                break;
            }
        }
        break;

        case _proc_step_t::SHOW_RESULT:
        {
            if(!(_showDuration.elapsed() < _opts.common.minDisplayResultTimeMs) /*&& !_result.isValid()*/){
                reset();
                if(cb_on_start)
                    cb_on_start();
            }
        }
        break;
        }

    }

    void Log(const QString & txt) {
        if(_cb_log)
            _cb_log("DocFrontProcessor: " + txt);
    }

    enum class _proc_step_t{
        COLLECT,
        SHOW_RESULT
    };

    Settings::Doc _opts;
    std::unique_ptr<ContourDetector>        _contour_detector;
    std::unique_ptr<FeatureDetector>        _feature_detector;
    std::unique_ptr<blur::BlurDetector>     _blur_detector;
    std::unique_ptr<glare::GlareDetector>   _glare_detector;
    std::function<void (const QString &)>   _cb_log;

    QElapsedTimer _detectDuration, _showDuration;
    _proc_step_t _proc_step = _proc_step_t::COLLECT;

    ProcResult _last_res;

    friend class Processor;
};


std::unique_ptr<Processor> Processor::create(const Settings::Doc &opts,
                                             std::unique_ptr<ContourDetector> contour_detector,
                                             std::unique_ptr<FeatureDetector> feature_detector,
                                             std::unique_ptr<blur::BlurDetector> blur_detector,
                                             std::unique_ptr<glare::GlareDetector> glare_detector,
                                             std::function<void (const QString &)> cb_log)
{
    return std::unique_ptr<Processor>(new DocFrontprocessorImpl(opts,
                                                                std::move(contour_detector),
                                                                std::move(feature_detector),
                                                                std::move(blur_detector),
                                                                std::move(glare_detector),
                                                                std::move(cb_log)));
}
