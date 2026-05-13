#ifndef DSL_STRINGCONTAINER_H_
#define DSL_STRINGCONTAINER_H_

#include"dsl_core.hpp"

namespace  rw {
	namespace dsl {
		class StringContainer {
		public:
			StringContainer();
			~StringContainer();
		public:
			virtual void insert(const String& key) = 0;
			virtual void remove(const String& key) = 0;
			virtual bool contains(const String& key) = 0;
			virtual size_t longentPrefix(String& key) = 0;
			virtual Vector<String> keysStartingWith(const String& prefix) = 0;
		};
		void testStringContainer();
	}
}

#endif // DSL_STRINGCONTAINER_H_