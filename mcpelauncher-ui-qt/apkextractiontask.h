#ifndef APKEXTRACTIONTASK_H
#define APKEXTRACTIONTASK_H

#include <QThread>
#include <QMutex>
#include <QTemporaryDir>

class VersionManager;

class ApkExtractionTask : public QThread {
    Q_OBJECT
    Q_PROPERTY(VersionManager* versionManager READ versionManager WRITE setVersionManager)
    Q_PROPERTY(QStringList sources READ sources WRITE setSources)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool allowIncompatible READ allowIncompatible WRITE SetAllowIncompatible)
    Q_PROPERTY(QString versionName READ versionName WRITE setVersionName)

    QMutex mutex;
    QStringList m_sources;
    VersionManager* m_versionManager;
    bool m_allowIncompatible;
    QString m_versionName;

    void run() override;

    void emitActiveChanged() {
        emit activeChanged();
    }

private slots:
    void onVersionInformationObtained(QString const& directory, QString const& versionName, int versionCode);


public:
    explicit ApkExtractionTask(QObject *parent = nullptr);

    bool active() const { return isRunning(); }

    QStringList sources() {
        QMutexLocker locker(&mutex);
        return m_sources;
    }

    void setSources(QStringList const& value) {
        QMutexLocker locker(&mutex);
        m_sources = value;
    }

    VersionManager* versionManager() {
        QMutexLocker locker(&mutex);
        return m_versionManager;
    }

    bool allowIncompatible() {
        return m_allowIncompatible;
    }

    void setVersionManager(VersionManager* value) {
        QMutexLocker locker(&mutex);
        m_versionManager = value;
    }

    QString versionName() {
        return m_versionName;
    }
    void setVersionName(QString versionName) {
        m_versionName = versionName;
    }
public slots:
    bool setSourceUrls(QList<QUrl> const& urls);
    void SetAllowIncompatible(bool c) {
        m_allowIncompatible = c;
    }

signals:
    void progress(qreal progress);

    void versionInformationObtained(QString const& directory, QString const& versionName, int versionCode);

    void finished();

    void error(QString const& err);

    void activeChanged();

};

#endif // APKEXTRACTIONTASK_H
