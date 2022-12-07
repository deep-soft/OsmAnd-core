#ifndef _OSMAND_CORE_I_WEB_CLIENT_H_
#define _OSMAND_CORE_I_WEB_CLIENT_H_

#include <OsmAndCore/stdlib_common.h>
#include <functional>

#include <OsmAndCore/QtExtensions.h>
#include <OsmAndCore/ignore_warnings_on_external_includes.h>
#include <QString>
#include <QByteArray>
#include <OsmAndCore/restore_internal_warnings.h>

#include <OsmAndCore.h>
#include <OsmAndCore/CommonSWIG.h>
#include <OsmAndCore/IQueryController.h>
#include <OsmAndCore/PrivateImplementation.h>
#include <OsmAndCore/IRequestResult.h>

namespace OsmAnd
{
    class OSMAND_CORE_API IWebClient
    {
        Q_DISABLE_COPY_AND_MOVE(IWebClient);

    public:
        typedef std::function<void(
            const uint64_t transferredBytes,
            const uint64_t totalBytes)> RequestProgressCallbackSignature;

    private:
    protected:
        IWebClient();
    public:
        virtual ~IWebClient();

        virtual QByteArray downloadData(
            const QString& url,
            std::shared_ptr<const IRequestResult>* const requestResult = nullptr,
            const SWIG_CLARIFY(IWebClient, RequestProgressCallbackSignature) progressCallback = nullptr,
            const std::shared_ptr<const IQueryController>& queryController = nullptr,
            const QString& userAgent = QString()) const = 0;
        virtual QString downloadString(
            const QString& url,
            std::shared_ptr<const IRequestResult>* const requestResult = nullptr,
            const SWIG_CLARIFY(IWebClient, RequestProgressCallbackSignature) progressCallback = nullptr,
            const std::shared_ptr<const IQueryController>& queryController = nullptr) const = 0;
        virtual bool downloadFile(
            const QString& url,
            const QString& fileName,
            std::shared_ptr<const IRequestResult>* const requestResult = nullptr,
            const SWIG_CLARIFY(IWebClient, RequestProgressCallbackSignature) progressCallback = nullptr,
            const std::shared_ptr<const IQueryController>& queryController = nullptr) const = 0;
    };

    SWIG_EMIT_DIRECTOR_BEGIN(IWebClient);
        SWIG_EMIT_DIRECTOR_CONST_METHOD(
            QByteArray,
            downloadData,
            const QString& url,
            std::shared_ptr<const IRequestResult>* const requestResult,
            const SWIG_CLARIFY(IWebClient, RequestProgressCallbackSignature) progressCallback,
            const std::shared_ptr<const IQueryController>& queryController,
            const QString& userAgent);
        SWIG_EMIT_DIRECTOR_CONST_METHOD(
            QString,
            downloadString,
            const QString& url,
            std::shared_ptr<const IRequestResult>* const requestResult,
            const SWIG_CLARIFY(IWebClient, RequestProgressCallbackSignature) progressCallback,
            const std::shared_ptr<const IQueryController>& queryController);
        SWIG_EMIT_DIRECTOR_CONST_METHOD(
            bool,
            downloadFile,
            const QString& url,
            const QString& fileName,
            std::shared_ptr<const IRequestResult>* const requestResult,
            const SWIG_CLARIFY(IWebClient, RequestProgressCallbackSignature) progressCallback,
            const std::shared_ptr<const IQueryController>& queryController);
    SWIG_EMIT_DIRECTOR_END(IWebClient);
}

#endif // !defined(_OSMAND_CORE_I_WEB_CLIENT_H_)

