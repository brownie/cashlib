#ifndef _ASTNODE_H_
#define _ASTNODE_H_

#include <string>
#include "ASTVisitor.h"
#include <vector>
#include <boost/lexical_cast.hpp>
#include <NTL/ZZ.h>
#include <utility>
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "new_ptr.hpp"

using namespace NTL;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using boost::lexical_cast;
using boost::dynamic_pointer_cast;
class Environment;

/*! struct useful for storing information about variables (which group
 * they are in and what their role is within the group) */
struct VarInfo {
	enum var_type { ELEMENT, EXPONENT, INTEGER, MODULUS };
	
	VarInfo() {}

	VarInfo(const string &g, const var_type t)
		: group(g), type(t) {}

	string group;
	var_type type;

	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int ver) {
		ar	& auto_nvp(group)
			& auto_nvp(type)
			;
	}
};

class ASTNode {
	public:	
		typedef boost::shared_ptr<ASTNode> ptr_t;
		virtual void visit(ASTVisitor& v) = 0;
		virtual void visitChildren(ASTVisitor& v) = 0;
		virtual ~ASTNode() {}

	protected:	
		/*! A helper method for building the visitChildren method
	 	 * when some constructions are optional */
		virtual void visitNonNull(ptr_t n, ASTVisitor& v) {
			if (n != 0)
				n->visit(v);
		}		
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {}
};
typedef ASTNode::ptr_t ASTNodePtr;
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ASTNode)

class ASTIdentifierLit : public ASTNode, 
	public boost::enable_shared_from_this<ASTIdentifierLit> {

	public:	
		typedef boost::shared_ptr<ASTIdentifierLit> ptr_t;
		/*! this class represents simple variable names, i.e. 'x' */	
		ASTIdentifierLit(string n, int l, int c) 
			: name(n), line(l), column(c) {}

		ASTIdentifierLit(const ASTIdentifierLit& o) 
			: name(o.name), line(o.line), column(o.column) {}

		ptr_t clone() const { return ptr_t(new ASTIdentifierLit(*this)); }
	
		string toString() const { return string("ID(")+name+")"; }
		string getName() const { return name; }
		int getLine() const { return line; }
		int getColumn() const { return column; }

		void visit(ASTVisitor& v) {	v.applyASTIdentifierLit(shared_from_this()); }
		void visitChildren(ASTVisitor& v) {}
		ZZ eval(Environment &env);

	private: 
		string name;
		int line;
		int column;		
		
		ASTIdentifierLit() {}
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTNode);
			ar	& auto_nvp(name)
				& auto_nvp(line)
				& auto_nvp(column)
				;
		}
};
typedef ASTIdentifierLit::ptr_t ASTIdentifierLitPtr;

class ASTIdentifierSub : public ASTNode, 
	public boost::enable_shared_from_this<ASTIdentifierSub> {

	public:	
		typedef boost::shared_ptr<ASTIdentifierSub> ptr_t; 
		/*! this class represents slightly more complex variable names, such
		 * as x_1 */
		ASTIdentifierSub(ASTIdentifierLitPtr b, string s) : base(b), sub(s) {}
	
		ASTIdentifierSub(const ASTIdentifierSub& o) : 
			base(o.base->clone()), sub(o.sub) {}

		ptr_t clone() const { return ptr_t(new ASTIdentifierSub(*this)); }
	
		string toString() {
			if(sub.empty())
				return base->toString();
			else 
				return base->toString()+"_"+sub;
		}	

		string getName() { return (sub.empty()) ? base->getName() : base->getName()+"_"+sub; }
		ASTIdentifierLitPtr getBase() { return base; }
		string getSub() const { return sub; }

		void visit(ASTVisitor& v) { v.applyASTIdentifierSub(shared_from_this()); }
		void visitChildren(ASTVisitor& v) { base->visit(v); }
		
		void setSub(string newSub) { sub = newSub; }		
		
		ZZ eval(Environment &env);
	
	private: 
		ASTIdentifierLitPtr base;
		string sub;

		ASTIdentifierSub() {}
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTNode);
			ar	& auto_nvp(base)
				& auto_nvp(sub)
				;
		}
};
typedef ASTIdentifierSub::ptr_t ASTIdentifierSubPtr;

class ASTDeclGeneral : public ASTNode {
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTNode);
		}
};
typedef ASTDeclGeneral::ptr_t ASTDeclGeneralPtr;

class ASTDeclIdentifierLit : public ASTDeclGeneral, 
	public boost::enable_shared_from_this<ASTDeclIdentifierLit> {

	public:	
		typedef boost::shared_ptr<ASTDeclIdentifierLit> ptr_t;
		/*! this class represents a variable name such as 'x' as it is being
		 * declared, either on the left-hand side of a declaration 
		 * (so 'x := a * b') or in a list, such as 'exponent in G: x' */
		ASTDeclIdentifierLit(string n, int l, int c) 
			: name(n), line(l), column(c) {}
	
		string toString() const { return "ID("+name+")"; }	
		string getName() const { return name; }
		int getLine() const { return line; }
		int getColumn() const { return column; }

		void visit(ASTVisitor& v) { v.applyASTDeclIdentifierLit(shared_from_this()); }
		void visitChildren(ASTVisitor& v) {}

	private:
		string name;
		int line;
		int column;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTDeclGeneral);
			ar	& auto_nvp(name)
				& auto_nvp(line)
				& auto_nvp(column)
				;
		}
};
typedef ASTDeclIdentifierLit::ptr_t ASTDeclIdentifierLitPtr;
typedef boost::shared_ptr<ASTIdentifierLit> ASTIdentifierLitPtr;

class ASTDeclIdentifierSub : public ASTDeclGeneral, 
	public boost::enable_shared_from_this<ASTDeclIdentifierSub> {

	public:	
		typedef boost::shared_ptr<ASTDeclIdentifierSub> ptr_t; 		
		/*! this class represents a variable name such as 'x_1' as it is being
		 * declared, either on the left-hand side of a declaration 
		 * (so 'x_1 := a * b') or in a list, such as 'exponent in G: x_1' */
		ASTDeclIdentifierSub(ASTIdentifierLitPtr b, string s) 
			: base(b), sub(s) {}
	
		ASTDeclIdentifierSub(const ASTDeclIdentifierSub& o) 
			: base(o.base->clone()){
			char* subclone = new char[o.sub.size()+1];
			strcpy(subclone, o.sub.c_str());
			sub = string(subclone);
		}
		ptr_t clone() {return ptr_t(new ASTDeclIdentifierSub(*this));}
	
		string toString() const { return (sub.empty()) ? base->toString() : base->toString()+"_"+sub; }
		string getName() const { return (sub.empty()) ? base->getName() : base->getName()+"_"+sub; }
		ASTIdentifierLitPtr getBase() { return base; }
		string getSub() const { return sub; }
		void setSub(string newSub) { sub = newSub; }

		void visit(ASTVisitor& v) { v.applyASTDeclIdentifierSub(shared_from_this()); }
		void visitChildren(ASTVisitor& v) { base->visit(v); }
	
	private:
		ASTIdentifierLitPtr base;
		string sub;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTDeclGeneral);
			ar	& auto_nvp(base)
				& auto_nvp(sub)
				;
		}
};
typedef ASTDeclIdentifierSub::ptr_t ASTDeclIdentifierSubPtr;

class ASTExpr : public ASTNode {

	public:	
		typedef boost::shared_ptr<ASTExpr> ptr_t;
		/*! this class represents the parent class for all our arithmetic
		 * expressions, as well as the variables that are used in those
		 * expressions */
		virtual ZZ eval(Environment &env){
			throw CashException(CashException::CE_PARSE_ERROR,
				"This function has not been instantiated!");	
		}

		virtual VarInfo getExprType(Environment &env) = 0;

		// want toString for insertion into environment maps
		virtual string toString() const = 0;

		// clone sub-trees via a "virtual constructor", as in
		// http://www.parashift.com/c++-faq-lite/virtual-functions.html#faq-20.8
		virtual boost::shared_ptr<ASTExpr> clone() const = 0;

	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTNode);
		}
};
typedef ASTExpr::ptr_t ASTExprPtr;
typedef boost::shared_ptr<ASTExpr> ASTExprPtr;
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ASTExpr)

class ASTExprInt : public ASTExpr, 
	public boost::enable_shared_from_this<ASTExprInt> {

	public:	
		typedef boost::shared_ptr<ASTExprInt> ptr_t; 
		/*! this class represents an integer literal such as '1' or '2' as
		 * it is being used in an arithmetic expression */
		ASTExprInt() {}
		ASTExprInt(ZZ i) : id(i) {}
		ASTExprInt(const string &i)	: id(i) {}

		ASTExprInt(const ASTExprInt& o) : id(o.id) {}
		ASTExprPtr clone() const { return ptr_t(new ASTExprInt(*this)); }
	
		void visit(ASTVisitor& v) {	v.applyASTExprInt(shared_from_this()); }
		void visitChildren(ASTVisitor& v) {}

		ZZ getVal() { return id; } 
		string toString() const { return lexical_cast<string>(id); }
		ZZ eval(Environment &env) { return id; }
		VarInfo getExprType(Environment &env);

	private: 
		ZZ id;		
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTExpr);
			ar  & auto_nvp(id)
				;
		}
};
typedef ASTExprInt::ptr_t ASTExprIntPtr;

class ASTExprIdentifier : public ASTExpr, 
	public boost::enable_shared_from_this<ASTExprIdentifier> {

	public:	
		typedef boost::shared_ptr<ASTExprIdentifier> ptr_t; 
		/*! this class represents a variable such as 'x' or 'x_1' as it
		 * is being used in an arithmetic expression */
		ASTExprIdentifier(ASTIdentifierSubPtr i) : id(i) {}

		ASTExprIdentifier(const ASTExprIdentifier& o) : id(o.id->clone()) {}
		ASTExprPtr clone() const { return ptr_t(new ASTExprIdentifier(*this)); }
	
		ASTExprIdentifier(const string &name) 
			: id(ASTIdentifierSubPtr(new ASTIdentifierSub(
				  ASTIdentifierLitPtr(new ASTIdentifierLit(name, -1, -1)), ""))) {}

		ASTIdentifierSubPtr getId() { return id; } 
		string getName() const { return id->getName(); }
		string toString() const { return id->getName(); }

		void visit(ASTVisitor& v) {	v.applyASTExprIdentifier(shared_from_this()); }
		void visitChildren(ASTVisitor& v) {	id->visit(v); }
		ZZ eval(Environment &env) { return id->eval(env); }
		VarInfo getExprType(Environment &env);

	private: 
		ASTIdentifierSubPtr id;

		ASTExprIdentifier() {}
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTExpr);
			ar  & auto_nvp(id)
				;
		}
};
typedef ASTExprIdentifier::ptr_t ASTExprIdentifierPtr;

class ASTUnaryOp : public ASTExpr {

	public:	
		typedef boost::shared_ptr<ASTUnaryOp> ptr_t;
		/*! this class represents an operation on one variable or expression */
		ASTUnaryOp(ASTExprPtr e) : expr(e) {}

		ASTUnaryOp(const ASTUnaryOp& o) : expr(o.expr->clone()) {}
	
		ASTExprPtr getExpr() { return expr; }
		void setExpr(ASTExprPtr newExpr) { expr = newExpr; }
	
		void visitChildren(ASTVisitor& v) { expr->visit(v); }

	protected:
		ASTExprPtr expr;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTExpr);
			ar	& auto_nvp(expr)
				;
		}
};
typedef ASTUnaryOp::ptr_t ASTUnaryOpPtr;


class ASTNegative : public ASTUnaryOp, 
	public boost::enable_shared_from_this<ASTNegative> {

	public:	
		typedef boost::shared_ptr<ASTNegative> ptr_t;
		/*! given a variable 'x' (or a more complex expression), this class
		 * represents -x */
		ASTNegative(ASTExprPtr expr) : ASTUnaryOp(expr) {}
	
		ASTExprPtr clone() const { return ptr_t(new ASTNegative(*this)); }

		void visit(ASTVisitor& v) {	v.applyASTNegative(shared_from_this()); }		
		
		string toString() const { return string("-"+expr->toString()); }

		ZZ eval(Environment &env) { return -(expr->eval(env)); }

		VarInfo getExprType(Environment &env);
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTUnaryOp);
		}

};
typedef ASTNegative::ptr_t ASTNegativePtr;

class ASTBinaryOp : public ASTExpr {

	public:	
		typedef boost::shared_ptr<ASTBinaryOp> ptr_t;
		/*! this class represents operations on two variables or expressions
		 * and so it stores a left-hand side and a right-hand side */
		ASTBinaryOp(ASTExprPtr lhs, ASTExprPtr rhs) : lhs(lhs), rhs(rhs) {}

		ASTBinaryOp(const ASTBinaryOp& o) 
			: lhs(o.lhs->clone()), rhs(o.rhs->clone()) {}
		
		ASTExprPtr getLHS() { return lhs; }
		ASTExprPtr getRHS() { return rhs; }
		void setLHS(ASTExprPtr newExpr) { lhs = newExpr; }
		void setRHS(ASTExprPtr newExpr) { rhs = newExpr; }
				
		void visitChildren(ASTVisitor& v){
			lhs->visit(v);
			rhs->visit(v);
		}
		VarInfo getExprType(Environment &env);

		pair<VarInfo,VarInfo> getTypes(Environment &env);

	protected: 
		ASTExprPtr lhs;
		ASTExprPtr rhs;

		ASTBinaryOp() {}
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTExpr);
			ar	& auto_nvp(lhs)
				& auto_nvp(rhs)
				;
		}
};
typedef ASTBinaryOp::ptr_t ASTBinaryOpPtr;

class ASTAdd : public ASTBinaryOp, 
	public boost::enable_shared_from_this<ASTAdd> {

	public:	
		typedef boost::shared_ptr<ASTAdd> ptr_t; 
		/*! this class represents the addition of its two input expressions */
		ASTAdd(ASTExprPtr lhs, ASTExprPtr rhs) : ASTBinaryOp(lhs,rhs) {}
		ASTExprPtr clone() const { return ptr_t(new ASTAdd(*this)); }
	
		void visit(ASTVisitor& v) {	v.applyASTAdd(shared_from_this()); }		
		
		string toString() const { 
			bool lExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(lhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(lhs) != 0);
			bool rExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(rhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(rhs) != 0);
			string ls = lExpr ? lhs->toString() : string("("+lhs->toString()+")");
			string rs = rExpr ? rhs->toString() : string("("+rhs->toString()+")");
			return string(ls+"+"+rs); 
		}

		ZZ eval(Environment &env);
		
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTBinaryOp);
		}

};
typedef ASTAdd::ptr_t ASTAddPtr;

class ASTSub : public ASTBinaryOp, 
	public boost::enable_shared_from_this<ASTSub> {

	public:	
		typedef boost::shared_ptr<ASTSub> ptr_t;
		/*! this class represents the subtraction of the right-hand side
		 * from the left-hand side */
	   	ASTSub(ASTExprPtr lhs, ASTExprPtr rhs) : ASTBinaryOp(lhs,rhs) {}
		ASTExprPtr clone() const { return ptr_t(new ASTSub(*this)); }
	
		void visit(ASTVisitor& v) {	v.applyASTSub(shared_from_this()); }		
				
		string toString() const { 			
			bool lExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(lhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(lhs) != 0);
			bool rExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(rhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(rhs) != 0);
			string ls = lExpr ? lhs->toString() : string("("+lhs->toString()+")");
			string rs = rExpr ? rhs->toString() : string("("+rhs->toString()+")");
			return string(ls+"-"+rs); 
		}
				
		ZZ eval(Environment &env);	

	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTBinaryOp);
		}

};
typedef ASTSub::ptr_t ASTSubPtr;

class ASTMul : public ASTBinaryOp, 
	public boost::enable_shared_from_this<ASTMul> {

	public:	
		typedef boost::shared_ptr<ASTMul> ptr_t;
		/*! this class represents the multiplication of its two input
		 * expressions */
	   	ASTMul(ASTExprPtr lhs, ASTExprPtr rhs) : ASTBinaryOp(lhs,rhs) {}
		ASTExprPtr clone() const { return ptr_t(new ASTMul(*this)); }
	
		void visit(ASTVisitor& v) {	v.applyASTMul(shared_from_this()); }
		
		string toString() const { 
			bool lExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(lhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(lhs) != 0);
			bool rExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(rhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(rhs) != 0);
			string ls = lExpr ? lhs->toString() : string("("+lhs->toString()+")");
			string rs = rExpr ? rhs->toString() : string("("+rhs->toString()+")");
			return string(ls+"*"+rs); 
		}
				
		ZZ eval(Environment &env);

	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTBinaryOp);
		}

};
typedef ASTMul::ptr_t ASTMulPtr;

class ASTDiv : public ASTBinaryOp, 
	public boost::enable_shared_from_this<ASTDiv> {

	public:	
		typedef boost::shared_ptr<ASTDiv> ptr_t;
		/*! this class represents the division of the right-hand side from
		 * the left-hand side */
		ASTDiv(ASTExprPtr lhs, ASTExprPtr rhs) : ASTBinaryOp(lhs,rhs) {}
		ASTExprPtr clone() const { return ptr_t(new ASTDiv(*this)); }
	
		void visit(ASTVisitor& v) { v.applyASTDiv(shared_from_this()); }
		
		string toString() const { 
			bool lExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(lhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(lhs) != 0);
			bool rExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(rhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(rhs) != 0);
			string ls = lExpr ? lhs->toString() 
							  : string("("+lhs->toString()+")");
			string rs = rExpr ? rhs->toString() 
							  : string("("+rhs->toString()+")");
			return string(ls+"/"+rs); 
		}
		
		ZZ eval(Environment &env);

	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTBinaryOp);
		}

};
typedef ASTDiv::ptr_t ASTDivPtr;

class ASTPow : public ASTBinaryOp, 
	public boost::enable_shared_from_this<ASTPow> {

	public:	
		typedef boost::shared_ptr<ASTPow> ptr_t;
		/*! this class represents raising the left-hand side to the 
		 * right-hand side */
		ASTPow(ASTExprPtr base, ASTExprPtr exp)	: ASTBinaryOp(base,exp) {}
		ASTExprPtr clone() const { return ptr_t(new ASTPow(*this)); }
	
		void visit(ASTVisitor& v) {	v.applyASTPow(shared_from_this()); }
				
		string toString() const { 
			bool lExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(lhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(lhs) != 0);
			bool rExpr = (dynamic_pointer_cast<ASTExprIdentifierPtr>(rhs) != 0 
						  || dynamic_pointer_cast<ASTExprIntPtr>(rhs) != 0);
			string ls = lExpr ? lhs->toString() : string("("+lhs->toString()+")");
			string rs = rExpr ? rhs->toString() : string("("+rhs->toString()+")");
			return string(ls+"^"+rs); 
		}
		
		ZZ eval(Environment &env);
		VarInfo getExprType(Environment &env);

	private:
		ASTPow() : ASTBinaryOp() {}
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTBinaryOp);
		}

};
typedef ASTPow::ptr_t ASTPowPtr;

class ASTList : public ASTNode, 
	public boost::enable_shared_from_this<ASTList> {

	public:	
		typedef boost::shared_ptr<ASTList> ptr_t;
		/*! this class is the parent class for any type of parameterized list */
   		ASTList() {}
		
    	bool isEmpty() const { return list.empty(); }
    	int size() const { return list.size(); }
    	void add(ASTNodePtr d) { list.push_back(d); }
    	ASTNodePtr get(int x) { return list[x]; }
		void setList(vector<ASTNodePtr> l) { list = l; }

		void visit(ASTVisitor& v) { v.applyASTList(shared_from_this()); }
    
		void visitChildren(ASTVisitor& v) {
			for (vector<ASTNodePtr>::iterator i = list.begin(); i != list.end(); ++i) { 
				(*i)->visit(v); 
			}
		}
    
	protected: 
		vector<ASTNodePtr> list;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTNode);
			ar	& auto_nvp(list)
				;
		}
};
typedef ASTList::ptr_t ASTListPtr;

class ASTListIdentifierLit : public ASTList {

	public:	
		typedef boost::shared_ptr<ASTListIdentifierLit> ptr_t; 
		/*! this class represents a list of variables 'x, y, z', etc */
    	ASTIdentifierLitPtr getLit(int x) 
			{ return dynamic_pointer_cast<ASTIdentifierLit>(list[x]); }
		void visit(ASTVisitor& v) 
			{ v.applyASTListIdentifierLit(dynamic_pointer_cast<ASTListIdentifierLit>(shared_from_this())); }

	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTList);
		}

};
typedef ASTListIdentifierLit::ptr_t ASTListIdentifierLitPtr;

class ASTListIdentifierSub : public ASTList {

	public:	
		typedef boost::shared_ptr<ASTListIdentifierSub> ptr_t;
		/*! this class represents a list of variables 'x_1, x_2', etc */
		ASTListIdentifierSub() {}

		ASTListIdentifierSub(const ASTListIdentifierSub& o) {
			ASTIdentifierSubPtr temp;
			for(unsigned x = 0; x<o.list.size(); x++){
				if((temp = dynamic_pointer_cast<ASTIdentifierSub>(o.list[x]))){
					add(temp->clone());
				} else {
					throw CashException(CashException::CE_PARSE_ERROR,
						"ASTListIdentifierSub contains a node which is "
						"not an ASTIdentifierSub");
				}
			}
		}
		
		ptr_t clone() const 
				{ return ptr_t(new ASTListIdentifierSub(*this)); }
	
		void visit(ASTVisitor& v) {
			v.applyASTListIdentifierSub(dynamic_pointer_cast<ASTListIdentifierSub>(shared_from_this()));
		}
	
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTList);
		}
};
typedef ASTListIdentifierSub::ptr_t ASTListIdentifierSubPtr;

class ASTListDecl : public ASTList {

	public:	
		typedef boost::shared_ptr<ASTListDecl> ptr_t;
		/*! this class is used as a parent for any list of declarations */
		void visit(ASTVisitor& v){ v.applyASTListDecl(dynamic_pointer_cast<ASTListDecl>(shared_from_this())); }

	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTList);
		}
};
typedef ASTListDecl::ptr_t ASTListDeclPtr;

class ASTListDeclIdentifierLit : public ASTListDecl {

	public:	
		typedef boost::shared_ptr<ASTListDeclIdentifierLit> ptr_t; 
		/*! this represents a list of variables 'x, y, z' that are 
		 * being declared within the list */
		void visit(ASTVisitor& v) {
			v.applyASTListDeclIdentifierLit(dynamic_pointer_cast<ASTListDeclIdentifierLit>(shared_from_this()));
		}
	
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTListDecl);
		}

};
typedef ASTListDeclIdentifierLit::ptr_t ASTListDeclIdentifierLitPtr;

class ASTListDeclIdentifierSub : public ASTListDecl {

	public:	
		typedef boost::shared_ptr<ASTListDeclIdentifierSub> ptr_t; 
		/*! this represents a list of variables 'x_1, x_2, x_3' that are 
		 * being declared within the list */
		void visit(ASTVisitor& v) {
			v.applyASTListDeclIdentifierSub(dynamic_pointer_cast<ASTListDeclIdentifierSub>(shared_from_this()));
		}
	
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTListDecl);
		}

};
typedef ASTListDeclIdentifierSub::ptr_t ASTListDeclIdentifierSubPtr;

class ASTDeclIDRange : public ASTDeclGeneral, 
	public boost::enable_shared_from_this<ASTDeclIDRange> {

	public:	
		typedef boost::shared_ptr<ASTDeclIDRange> ptr_t;
		/*! this represents a variable declaration of the form 'x[1:3]', which
		 * is shorthand for declaring 'x_1, x_2, x_3' */
		ASTDeclIDRange(ASTDeclIdentifierLitPtr b, ASTExprPtr lbound, ASTExprPtr ubound)
			: base(b), lbound(lbound), ubound(ubound) {}

		void visit(ASTVisitor& v) { v.applyASTDeclIDRange(shared_from_this()); }

		void visitChildren(ASTVisitor& v) { base->visit(v); } 

		ASTExprPtr getLP() { return lbound; }
		
		int getLBound(){
			ASTExprIntPtr temp = dynamic_pointer_cast<ASTExprInt>(lbound);
			if (temp) 
				return to_int(temp->getVal());
			else 
				throw CashException(CashException::CE_PARSE_ERROR,
				"ASTDeclIDRange: Constant propagation failed for %s",
				(lbound->toString()).c_str());
		}

		void setLBound(ASTExprPtr ne) { lbound = ne; }

		ASTExprPtr getUP() { return ubound; }

		int getUBound(){
			ASTExprIntPtr temp = dynamic_pointer_cast<ASTExprInt>(ubound);
			if (temp) 
				return to_int(temp->getVal());
			else 
				throw CashException(CashException::CE_PARSE_ERROR,
				"ASTDeclIDRange: Constant propagation failed for %s",
				(ubound->toString()).c_str());
		}

		void setUBound(ASTExprPtr ne) { ubound = ne; }

		string getName(int x) 
				{ return base->getName()+"_"+lexical_cast<string>(x); }
		ASTDeclIdentifierLitPtr getBase() { return base; }
		
	private:
		ASTDeclIdentifierLitPtr base;
		ASTExprPtr lbound;
		ASTExprPtr ubound;	

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTDeclGeneral);
			ar	& auto_nvp(base)
				& auto_nvp(lbound)
				& auto_nvp(ubound)
				;
		}
};
typedef ASTDeclIDRange::ptr_t ASTDeclIDRangePtr;

class ASTRelation : public ASTNode {

	public :
		typedef boost::shared_ptr<ASTRelation> ptr_t;
		/*! this class represents the relations between variables we will 
		 * want to be proving in our zero-knowledge proof */
		virtual ptr_t clone() const = 0;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTNode);
		}
};
typedef ASTRelation::ptr_t ASTRelationPtr;
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ASTRelation)

class ASTForRel : public ASTRelation, 
	public boost::enable_shared_from_this<ASTForRel> {

	public:	
		typedef boost::shared_ptr<ASTForRel> ptr_t;
		/*! this class represents a line of the form 
		 * 'for(i, 1:4, c_i = g^x_i * h^r_i)' */
		ASTForRel(ASTIdentifierLitPtr index, ASTExprPtr lbound, 
				  ASTExprPtr ubound, ASTRelationPtr rel)
			: index(index), ubound(ubound), lbound(lbound), rel(rel) {}
		
		ASTForRel(const ASTForRel& o)
			: index(o.index->clone()), ubound(o.ubound->clone()), 
			  lbound(o.lbound->clone()), rel(o.rel->clone()) {}
		
		ASTRelationPtr clone() const { return ptr_t(new ASTForRel(*this)); }
		
		ASTIdentifierLitPtr getIndex() { return index; }
		ASTExprPtr getLBound() { return lbound; }
		void setLBound(ASTExprPtr ne) { lbound = ne; }
		ASTExprPtr getUBound() { return ubound; }
		void setUBound(ASTExprPtr ne) { ubound = ne; }
		ASTRelationPtr getRelation() { return rel; }
		
		void visitChildren(ASTVisitor& v){
			lbound->visit(v);
			ubound->visit(v);
			rel->visit(v);
		}
		
		void visit(ASTVisitor &v) { v.applyASTForRel(shared_from_this()); }

	private:
		ASTIdentifierLitPtr index;
		ASTExprPtr ubound;
		ASTExprPtr lbound;
		ASTRelationPtr rel;
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTRelation);
			ar	& auto_nvp(index)
				& auto_nvp(ubound)
				& auto_nvp(lbound)
				& auto_nvp(rel)
				;
		}
};
typedef ASTForRel::ptr_t ASTForRelPtr;


class ASTEqual : public ASTRelation, 
	public boost::enable_shared_from_this<ASTEqual> {

	public:	
		typedef boost::shared_ptr<ASTEqual> ptr_t; 
		/*! this class represents a line of the form 'id = expr' */
		ASTEqual(ASTIdentifierSubPtr i, ASTExprPtr e) 
			: id(i), expr(e) {}
		
		ASTEqual(const ASTEqual& o) : id(o.id->clone()), expr(o.expr->clone()) {}
		ASTRelationPtr clone() const { return ptr_t(new ASTEqual(*this)); }
				
		ASTIdentifierSubPtr getId() { return id; }
		ASTExprPtr getExpr() { return expr; }
		void setExpr(ASTExprPtr newExpr) { expr = newExpr; }

		void visit(ASTVisitor& v) {	v.applyASTEqual(shared_from_this()); }

		void visitChildren(ASTVisitor& v) {
			id->visit(v);
			expr->visit(v);
		}
	
	private: 
		ASTIdentifierSubPtr id;
		ASTExprPtr expr;
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTRelation);
			ar	& auto_nvp(id)
				& auto_nvp(expr)
				;
		}
};
typedef ASTEqual::ptr_t ASTEqualPtr;

class ASTCommitment : public ASTRelation, 
	public boost::enable_shared_from_this<ASTCommitment> {

	public:	
		typedef boost::shared_ptr<ASTCommitment> ptr_t;
		/*! this class represents a line of the form 
		 * 'commitment to x: c_x = g^x * h^r_x' */
		ASTCommitment(ASTListIdentifierSubPtr c, ASTIdentifierSubPtr i, ASTExprPtr e) 
			: coms(c), id(i), expr(e) {}

		ASTCommitment(const ASTCommitment& o)
			: coms(o.coms->clone()), id(o.id->clone()), expr(o.expr->clone()) {}
		
		ASTRelationPtr clone() const { return ptr_t(new ASTCommitment(*this)); }

		ASTListIdentifierSubPtr getComs() { return coms; }
		ASTIdentifierSubPtr getId() { return id;}
		ASTExprPtr getExpr() { return expr;}
		void setExpr(ASTExprPtr ne){ expr = ne;}
	
		void visit(ASTVisitor& v) {	v.applyASTCommitment(shared_from_this()); }

		void visitChildren(ASTVisitor& v) {
			coms->visit(v);
			id->visit(v);
			expr->visit(v);
		}
	
	private:
		ASTListIdentifierSubPtr coms;	
		ASTIdentifierSubPtr id;
		ASTExprPtr expr;
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTRelation);
			ar	& auto_nvp(coms)
				& auto_nvp(id)
				& auto_nvp(expr)
				;
		}
};
typedef ASTCommitment::ptr_t ASTCommitmentPtr;

class ASTGiven : public ASTNode {

	public:	
		typedef boost::shared_ptr<ASTGiven> ptr_t;
		/*! this class is used to represent blocks of declarations */
		ASTGiven(ASTListDeclPtr items) : items(items) {}
	
		ASTListDeclPtr getItems() { return items; }
	
	protected:
		ASTListDeclPtr items;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar  & base_object_nvp(ASTNode);
			ar	& auto_nvp(items)
				;
		}
};
typedef ASTGiven::ptr_t ASTGivenPtr;

class ASTRandomPrime : public ASTGiven, 
	public boost::enable_shared_from_this<ASTRandomPrime> {

	public:	
		typedef boost::shared_ptr<ASTRandomPrime> ptr_t;
		/*! this class represents a line of the form 
		 * 'random prime(s) of length {length}: idList */
		ASTRandomPrime(ASTExprPtr length, ASTListDeclPtr idList) :
			ASTGiven(idList), length(length) {}
		
		ASTRandomPrime(const ASTRandomPrime& o) :
			ASTGiven(o.items), length(o.length){}
	
		ASTExprPtr getLength() { return length; }
		void setLength(ASTExprPtr ne) { length = ne; }
		
		ASTListDeclPtr getIds() { return items; }

		void visitChildren(ASTVisitor& v) { items->visit(v); }
		
		void visit(ASTVisitor &v) { v.applyASTRandomPrime(shared_from_this()); }
		
	private:
		ASTExprPtr length;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTGiven);
			ar	& auto_nvp(length)
				;
		}
};
typedef ASTRandomPrime::ptr_t ASTRandomPrimePtr;

class ASTRandomBnd : public ASTGiven, 
	public boost::enable_shared_from_this<ASTRandomBnd> {

	public:	
		typedef boost::shared_ptr<ASTRandomBnd> ptr_t;
		/*! this class represents a line of the form
		 * 'random integer(s) in [lbound, ubound): idList */
		ASTRandomBnd(ASTExprPtr lbound, ASTExprPtr ubound, ASTListDeclPtr idList) :
			ASTGiven(idList), lbound(lbound), ubound(ubound){}
		
		ASTRandomBnd(const ASTRandomBnd& o) :
			ASTGiven(o.items), lbound(o.lbound), ubound(o.ubound){}
	
		ASTExprPtr getLBound() { return lbound; }
		void setLBound(ASTExprPtr ne) { lbound = ne; }
		ASTExprPtr getUBound() { return ubound; }
		void setUBound(ASTExprPtr ne) { ubound = ne; }

		ASTListDeclPtr getIds() { return items; }

		void visitChildren(ASTVisitor& v) { items->visit(v); }
		
		void visit(ASTVisitor &v) { v.applyASTRandomBnd(shared_from_this()); }
		
	private:
		ASTExprPtr lbound;
		ASTExprPtr ubound;
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTGiven);
			ar	& auto_nvp(lbound)
				& auto_nvp(ubound)
				;
		}
};
typedef ASTRandomBnd::ptr_t ASTRandomBndPtr;

class ASTDeclGroup : public ASTGiven, 
	public boost::enable_shared_from_this<ASTDeclGroup> {

	public:	
		typedef boost::shared_ptr<ASTDeclGroup> ptr_t; 
		/*! this class represents a line of the form
		 * 'group: G = <g,h> 
		 *		modulus: N', where specifying both the generators and the 
		 * name of the modulus is optional */
		ASTDeclGroup(ASTDeclIdentifierLitPtr g, ASTListDeclPtr i,
			   		 ASTDeclIdentifierSubPtr m)	
			: ASTGiven(i), group(g), modulus(m) {}

		ASTDeclIdentifierLitPtr getGroup() { return group; }

		ASTListDeclPtr getItems() { return items; }
		ASTDeclIdentifierSubPtr getModulus() { return modulus; }

		void visit(ASTVisitor& v) {	v.applyASTDeclGroup(shared_from_this()); }

		void visitChildren(ASTVisitor& v){
			group->visit(v);
			visitNonNull(items, v);
			visitNonNull(modulus, v);
		}
	
	private: 
		ASTDeclIdentifierLitPtr group;
		ASTDeclIdentifierSubPtr modulus;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTGiven);
			ar	& auto_nvp(group)
				& auto_nvp(modulus)
				;
		}
};
typedef ASTDeclGroup::ptr_t ASTDeclGroupPtr;

class ASTDeclIntegers : public ASTGiven, 
	public boost::enable_shared_from_this<ASTDeclIntegers> {

	public:	
		typedef boost::shared_ptr<ASTDeclIntegers> ptr_t; 
		/*! this class represents a list such as 'integers: stat, m, l_x' */
		ASTDeclIntegers(ASTListDeclPtr i) 
			: ASTGiven(i) {}
	
		ASTListDeclPtr getInts() { return items; }
	
		void visit(ASTVisitor& v) { v.applyASTDeclIntegers(shared_from_this());	}

		void visitChildren(ASTVisitor& v) { items->visit(v); }		
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTGiven);
		}

};
typedef ASTDeclIntegers::ptr_t ASTDeclIntegersPtr;

class ASTListGiven : public ASTList {

	public:	
		typedef boost::shared_ptr<ASTListGiven> ptr_t; 
		/*! this class represents a list of declarations that appear in
		 * the given block of a program */
		void visit(ASTVisitor& v) {
			v.applyASTListGiven(dynamic_pointer_cast<ASTListGiven>(shared_from_this()));
		}
	
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTList);
		}

};
typedef ASTListGiven::ptr_t ASTListGivenPtr;

class ASTListRandoms : public ASTList {

	public:	
		typedef boost::shared_ptr<ASTListRandoms> ptr_t; 
		/*! this class represents a collection of random computations, 
		 * such as computing random primes, computing random integers in 
		 * a given range, and computing random exponents in a given group */
		void visit(ASTVisitor& v) {
			v.applyASTListRandoms(dynamic_pointer_cast<ASTListRandoms>(shared_from_this()));
		}
	
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTList);
		}

};
typedef ASTListRandoms::ptr_t ASTListRandomsPtr;

class ASTListRelation : public ASTList {

	public:	
		typedef boost::shared_ptr<ASTListRelation> ptr_t; 
		/*! this class represents a list of relations that will appear in
		 * the 'such that' block of the program */
		void visit(ASTVisitor& v) {
			v.applyASTListRelation(dynamic_pointer_cast<ASTListRelation>(shared_from_this()));
		}
	
	private:
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTList);
		}

};
typedef ASTListRelation::ptr_t ASTListRelationPtr;

class ASTDeclElements : public ASTGiven, 
	public boost::enable_shared_from_this<ASTDeclElements> {

	public:	
		typedef boost::shared_ptr<ASTDeclElements> ptr_t; 
		/*! this class represents a declaration of group elements such as
		 * 'elements in G: A, B, c[1:3]', with an optional area for declaring
		 * relations (specifically for specifying the forms of commitments) */
		ASTDeclElements(ASTIdentifierLitPtr g, ASTListDeclPtr e, 
						ASTListRelationPtr r) 
			: ASTGiven(e), group(g), relations(r) {}
	
		ASTIdentifierLitPtr getGroup() { return group; }
		ASTListDeclPtr getElements() { return items; }
		ASTListRelationPtr getRelations() { return relations; }
	
		void visit(ASTVisitor& v) {	v.applyASTDeclElements(shared_from_this());	}
	
		void visitChildren(ASTVisitor& v){
			group->visit(v);
			items->visit(v);
			visitNonNull(relations, v);
		}
	
	private: 
		ASTIdentifierLitPtr group;
		ASTListRelationPtr relations;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTGiven);
			ar	& auto_nvp(group)
				& auto_nvp(relations)
				;
		}
};
typedef ASTDeclElements::ptr_t ASTDeclElementsPtr;

class ASTDeclExponents : public ASTGiven, 
	public boost::enable_shared_from_this<ASTDeclExponents> {

	public:	
		typedef boost::shared_ptr<ASTDeclExponents> ptr_t; 
		/*! this class represents a list of declarations of exponents, such as
		 * 'exponents in G: a, b, c' */
		ASTDeclExponents(ASTIdentifierLitPtr g, ASTListDeclPtr e) 
			: ASTGiven(e), group(g) {}
	
		ASTIdentifierLitPtr getGroup() { return group; }
		ASTListDeclPtr getExponents() { return items; }
	
		void visit(ASTVisitor& v) {	v.applyASTDeclExponents(shared_from_this()); }

		void visitChildren(ASTVisitor& v){
			group->visit(v);
			items->visit(v);
		}

	private: 
		ASTIdentifierLitPtr group;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTGiven);
			ar	& auto_nvp(group)
				;
		}
};
typedef ASTDeclExponents::ptr_t ASTDeclExponentsPtr;

class ASTDeclRandExponents : public ASTGiven, 
	public boost::enable_shared_from_this<ASTDeclRandExponents> {

	public:	
		typedef boost::shared_ptr<ASTDeclRandExponents> ptr_t;
		/*! this class represents a list of declarations of random exponents
		 * (only to be used in the compute block), such as
		 * 'random exponents in G: r_a, r_b, r_c' */
		ASTDeclRandExponents(ASTIdentifierLitPtr g, ASTListDeclPtr e) 
			: ASTGiven(e), group(g) {}
	
		ASTIdentifierLitPtr getGroup() { return group; }
		ASTListDeclPtr getExponents() { return items; }
	
		void visit(ASTVisitor& v) {	v.applyASTDeclRandExponents(shared_from_this()); }

		void visitChildren(ASTVisitor& v){
			group->visit(v);
			items->visit(v);
		}

	private: 
		ASTIdentifierLitPtr group;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTGiven);
			ar	& auto_nvp(group)
				;
		}
};
typedef ASTDeclRandExponents::ptr_t ASTDeclRandExponentsPtr;

class ASTDeclEqual : public ASTRelation, 
	public boost::enable_shared_from_this<ASTDeclEqual> {

	public:	
		typedef boost::shared_ptr<ASTDeclEqual> ptr_t; 
		/*! this class represents a line of the form 'x := a * b */
		ASTDeclEqual(ASTDeclIdentifierSubPtr i, ASTExprPtr e) 
			: id(i), expr(e) {}
	
		ASTDeclEqual(const ASTDeclEqual& o) 
			: id(o.id->clone()), expr(o.expr->clone()) {}

		ASTRelationPtr clone() const { return ptr_t(new ASTDeclEqual(*this)); }
	
		ASTDeclIdentifierSubPtr getId() { return id; }
		ASTExprPtr getExpr() { return expr; }
		void setExpr(ASTExprPtr newExpr){ expr = newExpr; }

		void visit(ASTVisitor& v) {	v.applyASTDeclEqual(shared_from_this()); }

		void visitChildren(ASTVisitor& v) {
			id->visit(v);
			expr->visit(v);
		}

	private: 
		ASTDeclIdentifierSubPtr id;
		ASTExprPtr expr;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTRelation);
			ar	& auto_nvp(id)
				& auto_nvp(expr)
				;
		}
};
typedef ASTDeclEqual::ptr_t ASTDeclEqualPtr;

class ASTRange : public ASTRelation,
	public boost::enable_shared_from_this<ASTRange> {

	public:	
		typedef boost::shared_ptr<ASTRange> ptr_t; 
		/*! this class is the parent for two types of ranges */
		ASTRange(ASTIdentifierLitPtr g, ASTExprPtr l, bool ls, ASTExprPtr c, 
				 bool us, ASTExprPtr u) 
			: group(g), lower(l), center(c), upper(u), lowerStrict(ls), 
			  upperStrict(us) {}
	
		ASTRange(const ASTRange& o) 
			: group(o.group->clone()), lower(o.lower->clone()), 
			  center(o.center->clone()), upper(o.upper->clone()), 
			  lowerStrict(o.lowerStrict), upperStrict(o.upperStrict) {}

		ASTRelationPtr clone() const { return ptr_t(new ASTRange(*this)); }
	
		ASTIdentifierLitPtr getGroup() { return group; }
		ASTExprPtr getLower() { return lower; }
		void setLower(ASTExprPtr ne) { lower = ne; }
		ASTExprPtr getCenter() { return center; }
		void setCenter(ASTExprPtr ne) { center = ne; }
		ASTExprPtr getUpper() { return upper; }
		void setUpper(ASTExprPtr ne) { upper = ne; }
		bool isLowerStrict() { return lowerStrict; }
		bool isUpperStrict() { return upperStrict; }
		
		void visit(ASTVisitor& v) {	v.applyASTRange(shared_from_this()); }
		
		void visitChildren(ASTVisitor& v) {
			group->visit(v);
			lower->visit(v);
			center->visit(v);
			upper->visit(v);
		}
	
	private:
		ASTIdentifierLitPtr group;
		ASTExprPtr lower;
		ASTExprPtr center;
		ASTExprPtr upper;
		bool lowerStrict;
		bool upperStrict;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTRelation);
			ar	& auto_nvp(lower)
				& auto_nvp(center)
				& auto_nvp(upper)
				& auto_nvp(lowerStrict)
				& auto_nvp(upperStrict)
				;
		}
};
typedef ASTRange::ptr_t ASTRangePtr;

class ASTForExpr : public ASTExpr, 
	public boost::enable_shared_from_this<ASTForExpr> {

	public:	
		typedef boost::shared_ptr<ASTForExpr> ptr_t;
		/*! this class represents a line of the form 
		 * 'for(i, 1:3, *, g^x_i)' */
		ASTForExpr(ASTIdentifierLitPtr i, ASTExprPtr l, ASTExprPtr u, 
				   string op, ASTExprPtr e)
			: index(i), lbound(l), ubound(u),  expr(e), op(op){}

		ASTForExpr(const ASTForExpr& o)
			: index(o.index->clone()), lbound(o.lbound->clone()), 
			  ubound(o.ubound->clone()), expr(o.expr->clone()), op(o.op) {}
		
		ASTExprPtr clone() const { return ptr_t(new ASTForExpr(*this)); }
		
		string toString() const { 
	   		throw CashException(CashException::CE_PARSE_ERROR,
								"A for node should never be visited");	
		}

		ASTIdentifierLitPtr getIndex() { return index; }
		ASTExprPtr getLBound() { return lbound; }
		void setLBound(ASTExprPtr ne) { lbound = ne; }
		ASTExprPtr getUBound() { return ubound; }
		void setUBound(ASTExprPtr ne) { ubound = ne; }
		ASTExprPtr getExpr() { return expr; }
		void setExpr(ASTExprPtr newExpr) { expr = newExpr; }
		string getOp() { return op; }
		
		void visitChildren(ASTVisitor& v){
			lbound->visit(v);
			ubound->visit(v);
			expr->visit(v);
		}
		
		void visit(ASTVisitor &v){ v.applyASTForExpr(shared_from_this()); }
		
		ZZ eval(Environment& e) { 
			throw CashException(CashException::CE_PARSE_ERROR,
								"Can't eval a for node");
		}
		
		VarInfo getExprType(Environment& e) {
			throw CashException(CashException::CE_PARSE_ERROR,
								"Cannot get type for a for node");
		}
		
	private:  
		ASTIdentifierLitPtr index;
		ASTExprPtr lbound;
		ASTExprPtr ubound;
		ASTExprPtr expr;
		string op;
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTExpr);
			ar	& auto_nvp(index)
				& auto_nvp(lbound)
				& auto_nvp(ubound)
				& auto_nvp(expr)
				& auto_nvp(op)
				;
		}
};
typedef ASTForExpr::ptr_t ASTForExprPtr;

class ASTComputation : public ASTNode, 
	public boost::enable_shared_from_this<ASTComputation> {

	public:	
		typedef boost::shared_ptr<ASTComputation> ptr_t; 
		/*! this class represents an entire computation block, in particular 
		 * the given block, and the two parts of the compute block: the 
		 * random computations and the declEqual computations */
		ASTComputation(ASTListGivenPtr g, ASTListRandomsPtr re, ASTListRelationPtr r) 
			: given(g), randExp(re), relations(r) {}
	
		ASTListGivenPtr getGiven() { return given; }
		ASTListRandomsPtr getRandExponents() { return randExp; }
		ASTListRelationPtr getRelations() { return relations; }

		void visit(ASTVisitor& v) {	v.applyASTComputation(shared_from_this()); }
	
	 	void visitChildren(ASTVisitor& v){
			given->visit(v);
			randExp->visit(v);
			relations->visit(v);
		}
	
	private: 
		ASTListGivenPtr given;
		ASTListRandomsPtr randExp;
		ASTListRelationPtr relations;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTNode);
			ar	& auto_nvp(given)
				& auto_nvp(randExp)
				& auto_nvp(relations)
				;
		}
};
typedef ASTComputation::ptr_t ASTComputationPtr;

class ASTProof : public ASTNode, 
	public boost::enable_shared_from_this<ASTProof> {

	public:	
		typedef boost::shared_ptr<ASTProof> ptr_t; 
		/*! this class represents an entire proof block: the given block,
		 * the 'prove knowledge of' block, and the 'such that' block */
		ASTProof(ASTListGivenPtr g, ASTListGivenPtr k, ASTListRelationPtr s) 
			: given(g), knowledge(k), suchThat(s) {}
	
		ASTListGivenPtr getGiven() { return given; }
		ASTListGivenPtr getKnowledge() { return knowledge; }
		ASTListRelationPtr getSuchThat() { return suchThat; }

		void visit(ASTVisitor& v) {	v.applyASTProof(shared_from_this()); }
	
		void visitChildren(ASTVisitor& v){
			given->visit(v);
			knowledge->visit(v);
			suchThat->visit(v);
		}

	private: 
		ASTListGivenPtr given;
		ASTListGivenPtr knowledge;
		ASTListRelationPtr suchThat;
		
		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTNode);
			ar	& auto_nvp(given)
				& auto_nvp(knowledge)
				& auto_nvp(suchThat)
				;
		}
};
typedef ASTProof::ptr_t ASTProofPtr;

class ASTSpec : public ASTNode, 
	public boost::enable_shared_from_this<ASTSpec> {

	public:	
		typedef boost::shared_ptr<ASTSpec> ptr_t; 
		/*! this class represents the whole program, consisting of the
		 * computation block and the proof block (both of which are 
		 * optional) */
		ASTSpec(ASTComputationPtr c, ASTProofPtr p) 
			: comp(c), proof(p) {}
	
		ASTComputationPtr getComputation() { return comp; }
		ASTProofPtr getProof() { return proof; }

		void visit(ASTVisitor& v) {	v.applyASTSpec(shared_from_this());	}
	
		void visitChildren(ASTVisitor& v){
			visitNonNull(comp, v);
			visitNonNull(proof, v);
		}
	
	private: 
		ASTComputationPtr comp;
		ASTProofPtr proof;

		friend class boost::serialization::access;
		template <class A> void serialize(A& ar, const unsigned int ver) {
			ar	& base_object_nvp(ASTNode);
			ar	& auto_nvp(comp)
				& auto_nvp(proof)
				;
		}
};
typedef ASTSpec::ptr_t ASTSpecPtr;

#endif /* _ASTNODE_H_*/
