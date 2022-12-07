#ifndef _OSMAND_CORE_I_HTTP_REQUEST_RESULT_H
#define _OSMAND_CORE_I_HTTP_REQUEST_RESULT_H

#include <OsmAndCore/QtExtensions.h>
#include <QString>

#include <OsmAndCore.h>
#include <OsmAndCore/CommonSWIG.h>
#include <OsmAndCore/IRequestResult.h>

namespace OsmAnd
{
    class OSMAND_CORE_API IHttpRequestResult : public IRequestResult
    {
        Q_DISABLE_COPY_AND_MOVE(IHttpRequestResult);

    private:
    protected:
        IHttpRequestResult();
    public:
        virtual ~IHttpRequestResult();

        virtual bool isSuccessful() const = 0;
        virtual unsigned int getHttpStatusCode() const = 0;
        virtual QString getLastModifiedHeaderValue() const = 0;
    };

    SWIG_EMIT_DIRECTOR_BEGIN(IHttpRequestResult);
        SWIG_EMIT_DIRECTOR_CONST_METHOD_NO_ARGS(bool, isSuccessful);
        SWIG_EMIT_DIRECTOR_CONST_METHOD_NO_ARGS(unsigned int, getHttpStatusCode);
        SWIG_EMIT_DIRECTOR_CONST_METHOD_NO_ARGS(QString, getLastModifiedHeaderValue);
    SWIG_EMIT_DIRECTOR_END(IHttpRequestResult);
}

#endif // !defined(_OSMAND_CORE_I_HTTP_REQUEST_RESULT_H)