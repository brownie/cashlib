
#ifndef _INTERPRETERCACHE_H_
#define _INTERPRETERCACHE_H_

#include "ASTNode.h"
#include "Environment.h"
#include "Interpreter.h"

/*! 
 * \brief A storage container that keeps previously "compiled" code 
 * (AST & Environment) for the interpreter to load up again as it wants
 */

struct CacheValue {

	CacheValue() {}

	CacheValue(ASTNodePtr n, const Environment& e)
		: tree(n), env(e) {}

	ASTNodePtr tree;
	Environment env;
};

class InterpreterCache {

	public:
        /*! return a thread-local instance of the InterpreterCache.
         * this should prevent different threads from invoking the
         * same interpreter. */
        static InterpreterCache& instance();
		
		typedef boost::unordered_map<cache_key_pair, CacheValue> cache_t;

		// store values in the cache
		static void store(cache_key_pair key, ASTNodePtr n,
						  Environment &env) {
			instance().cache[key] = CacheValue(n, env);
		}

		static bool contains(cache_key_pair key) {
			return instance().cache.count(key) != 0;
		}

		static CacheValue& get(cache_key_pair key) {
			return instance().cache.at(key);
		}
		
	private:
		cache_t cache;
	
		InterpreterCache() {} // constructor (ctor) is hidden
		InterpreterCache( InterpreterCache const & );  // copy ctor is hidden
		// assignment operator is hidden
		InterpreterCache& operator=( InterpreterCache const & );  
};

#endif /*_INTERPRETERCACHE_H_*/
