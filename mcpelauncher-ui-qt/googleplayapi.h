#ifndef GOOGLEPLAYAPI_H
#define GOOGLEPLAYAPI_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QSettings>
#include <future>
#include <playapi/api.h>

class GoogleLoginHelper;

class GooglePlayApi : public QObject {
    Q_OBJECT
    Q_PROPERTY(GoogleLoginHelper* login READ getLogin WRITE setLogin)
    Q_PROPERTY(GooglePlayApiStatus status READ getStatus NOTIFY statusChanged)

public:
    enum class GooglePlayApiStatus {
        NOT_READY, PENDING, FAILED, SUCCEDED
    };
    Q_ENUM(GooglePlayApiStatus)

private:
    QScopedPointer<playapi::api> api;
    GoogleLoginHelper* loginHelper = nullptr;
    QMutex checkinMutex;
    playapi::checkin_result checkinResult;
    std::promise<std::pair<bool, bool>> tosApprovalPromise;
    GooglePlayApiStatus status = GooglePlayApiStatus::NOT_READY;

    QString CheckinInfoGroup();
    void loadCheckinInfo();
    void saveCheckinInfo();

    void loadApiInfo();
    void saveApiInfo();

    void cleanupLogin();

    void setStatus(GooglePlayApiStatus status) {
        if (this->status != status) {
            this->status = status;
            statusChanged();
        }
    }

    void updateLogin();

public:
    explicit GooglePlayApi(QObject *parent = nullptr);

    void setLogin(GoogleLoginHelper* helper);

    GoogleLoginHelper* getLogin() { return loginHelper; }

    playapi::api* getApi() { return api.data(); }

    GooglePlayApiStatus getStatus() const { return status; }

signals:
    void ready();

    void initError(QString const& text);

    void tosApprovalRequired(QString const& tosText, QString const& marketingText);

    void appInfoReceived(QString const& packageName, QString const& version, int versionCode, bool isBeta);

    void appInfoFailed(QString const& packageName, QString errorMessage);

    void statusChanged();

public slots:
    void requestAppInfo(QString const& packageName);

    void validateLicense(std::string packagename, int versionscode, std::function<void (bool)> callback);

    void setTosApproved(bool approved, bool marketing) {
        tosApprovalPromise.set_value({approved, marketing});
    }

};

#endif // GOOGLEPLAYAPI_H
