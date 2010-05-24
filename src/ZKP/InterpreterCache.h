
#ifndef _INTERPRETERCACHE_H_
#define _INTERPRETERCACHE_H_

#include "ASTNode.h"
#include "Environment.h"

/*! 
 * \brief A storage container that keeps previously "compiled" code 
 * (AST & Environment) for the interpreter to load up again as it wants
 */

struct CacheValue {

	CacheValue() {}

	CacheValue(ASTNodePtr n, const Environment& e, const input_map& i)
		: tree(n), env(e), inputs(i) {}

	ASTNodePtr tree;
	Environment env;
	input_map inputs;	
};

class InterpreterCache {

	public:
		static InterpreterCache& instance() {
			// local static object initialization
			static InterpreterCache _icache;  
			return _icache;
		}
		
		typedef boost::unordered_map<string, CacheValue> cache_t;

		// store values in the cache
		static void store(const string &fname, input_map i, ASTNodePtr n,
						  Environment &env) {
			instance().cache[fname] = CacheValue(n, env, i);
		}

		static bool contains(const string& fname) {
			return instance().cache.count(fname) != 0;
		}

		static CacheValue& get(const string &fname) {
			return instance().cache.at(fname);
		}
		
	private:
		cache_t cache;
	
		InterpreterCache() {} // constructor (ctor) is hidden
		InterpreterCache( InterpreterCache const & );  // copy ctor is hidden
		// assignment operator is hidden
		InterpreterCache& operator=( InterpreterCache const & );  
		~InterpreterCache() {}  // destructor (dtor) is hidden
};

#endif /*_INTERPRETERCACHE_H_*/
