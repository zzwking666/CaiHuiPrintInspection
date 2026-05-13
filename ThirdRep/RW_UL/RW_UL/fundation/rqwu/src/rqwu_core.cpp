#include "rqwu/rqwu_core.h"
#include <QtCore/QResource>

namespace rw {
    namespace rqwu {
    	class QtResourceAutoInit {
        public:
            QtResourceAutoInit() { Q_INIT_RESOURCE(rqwu); }
        };

        void ini() {
            static QtResourceAutoInit _autoInit;
        }

    } // namespace rqwu
} // namespace rw