

#include <QGuiApplication>
#include <QFile>
#include <QTextStream>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
#include <QFontDatabase>
#include <QQmlContext>
#include <QDir>
#include <QIcon>
#include <QCommandLineParser>

#include "mainprocess.h"
#include "imageprovider.h"
#include "comboboxmodel.h"

// for console
#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{

#ifdef Q_OS_WIN
    // Присоединяемся к консоли родительского процесса
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        // Перенаправляем стандартные потоки (stdout/stderr)
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);


    //Command Line Parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Demo app");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption configOption(QStringList() << "c" << "config",
                                    QCoreApplication::translate("main", "Set config-file <json config file>."),
                                    QCoreApplication::translate("main", "./config.json"));
    QCommandLineOption defaultConfigOption(QStringList() << "d" << "default-config",
                                    QCoreApplication::translate("main", "Create default config-file <json config file>."),
                                    QCoreApplication::translate("main", "./config.json"));
    parser.addOption(configOption);
    parser.addOption(defaultConfigOption);
    parser.process(app);

    QString defaultConfigFile  = parser.value(defaultConfigOption);
    if(!defaultConfigFile.isEmpty()){
        if(QFile::exists(defaultConfigFile)){
            qCritical() << "Destination config file: " << defaultConfigFile << " is exists";
            return -1;
        }
        if(QFile::copy(":/res/config.json", defaultConfigFile))
            qDebug() << "Default cfg file is created: " << defaultConfigFile;
        else
            qCritical() << "Can't create file: " << defaultConfigFile;
        return 0;
    }

    QString configFile = parser.value(configOption);
    if(configFile.isEmpty()){
        qCritical() << "Please set config file with opt \"--config\" or create default config with opt \"--default-config\"";
        return -1;
    }


    app.setWindowIcon(QIcon("://app_icon.svg"));

    QSurfaceFormat format;
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);


    QObject * rootQmlObject= nullptr;
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    //add fonts
    for(const auto & font : QDir("://font/").entryList()){

        const auto font_path = "://font/" +font;
        if(-1 == QFontDatabase::addApplicationFont(font_path))
            qCritical() << "couldn't add application font : " << font_path;
    }

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url,&rootQmlObject](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
                else
                    rootQmlObject = obj;
            }, Qt::QueuedConnection);

    ImageProvider *  imgProvider = new ImageProvider(QQuickImageProvider::Image);

    //The QQmlEngine takes ownership of provider.
    engine.addImageProvider(QLatin1String("PhotosProvider"), imgProvider);

    QQmlContext * context = engine.rootContext();

    int result = -1;

    try {
        //auto cfg = JsonCfg::create("/home/anton/work/projects/FaceDetectDemo/FaceDetector/res/config.json");
        auto cfg = JsonCfg::create(configFile.toStdString());
        //время жизни класса MainWindow меньше чем ImageProvider, т.к. ImageProvider - сырой указатель
        {
            MainProcess w(imgProvider, std::move(cfg), nullptr);
            context->setContextProperty("cppHandler",      w.getCppHandler());
            context->setContextProperty("webcamListModel", w.getCppHandler()->getWebcamModels());

            engine.load(url);
            result = app.exec();
        }
    }
    catch(const std::exception & e){
        qDebug() << "main: exception: " << e.what();
    }

    return result;
}


