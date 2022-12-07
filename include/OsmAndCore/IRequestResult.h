#ifndef _OSMAND_CORE_I_REQUEST_RESULT_H
#define _OSMAND_CORE_I_REQUEST_RESULT_H

#include <OsmAndCore.h>
#include <OsmAndCore/CommonSWIG.h>

namespace OsmAnd
{
    class OSMAND_CORE_API IRequestResult
    {
        Q_DISABLE_COPY_AND_MOVE(IRequestResult);

    private:
    protected:
        IRequestResult();
    public:
        virtual ~IRequestResult();

        virtual bool isSuccessful() const = 0;
    };

    SWIG_EMIT_DIRECTOR_BEGIN(IRequestResult);
        SWIG_EMIT_DIRECTOR_CONST_METHOD_NO_ARGS(bool, isSuccessful);
    SWIG_EMIT_DIRECTOR_END(IRequestResult);
}

#endif // !defined(_OSMAND_CORE_I_REQUEST_RESULT_H)