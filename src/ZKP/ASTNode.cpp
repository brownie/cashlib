#include "ASTNode.h"
#include "Environment.h"


typedef VarInfo::var_type Type;

//
// getExprType(): returns type of expression (VarInfo)
// eval(): returns an integer, given an Environment (ZZ)
//

ZZ ASTIdentifierLit::eval(Environment &env) { 
	return env.variables.at(getName()); 
}

ZZ ASTIdentifierSub::eval(Environment &env) { 
	return env.variables.at(getName()); 
}

VarInfo ASTExprInt::getExprType(Environment &env) {
	if (env.exprTypes.count(toString()) == 0) {
		VarInfo info(Environment::NO_GROUP, VarInfo::INTEGER);
		env.exprTypes[toString()] = info;
		return info;
	} else {
		return env.exprTypes.at(toString());
	}
}

VarInfo ASTExprIdentifier::getExprType(Environment &env) {
	if (env.exprTypes.count(id->getName()) == 0) {
		VarInfo info = env.varTypes.at(id->getName());
		env.exprTypes[toString()] = info;
		return info;
	} else {
		return env.exprTypes.at(id->getName());
	}
}

VarInfo ASTNegative::getExprType(Environment &env) {
	if (env.exprTypes.count(expr->toString()) == 0) {
		VarInfo info = expr->getExprType(env);
		// this and underlying expr have same type
		env.exprTypes[expr->toString()] = info;
		env.exprTypes[toString()] = info;
		return info;
	} else {
		VarInfo info = env.exprTypes.at(expr->toString());
		env.exprTypes[toString()] = info;
		return info;
	}
}		

VarInfo ASTBinaryOp::getExprType(Environment &env) {
	VarInfo leftInfo;
	VarInfo rightInfo;
	// don't recompute types if we don't have to
	if (env.exprTypes.count(lhs->toString()) == 0) 
		leftInfo = lhs->getExprType(env);
	else 
		leftInfo = env.exprTypes.at(lhs->toString());
	if (env.exprTypes.count(rhs->toString()) == 0)
		rightInfo = rhs->getExprType(env);
	else
		rightInfo = env.exprTypes.at(rhs->toString());
	Type left = leftInfo.type;
	Type right = rightInfo.type;
	string lGrp = leftInfo.group;
	string rGrp = rightInfo.group;

	// integers can be mixed with anything without changing type
	// so can moduli (which are just integers)
	if (left == VarInfo::INTEGER || left == VarInfo::MODULUS) {
		VarInfo info(rGrp, right);
		env.exprTypes[toString()] = info;
		return info;
	}	
	else if (right == VarInfo::INTEGER || right == VarInfo::MODULUS) {
		VarInfo info(lGrp, left);
		env.exprTypes[toString()] = info;
		return info;
	}
	// exponents can be mixed in the same group
	else if (left == right && lGrp == rGrp && 
			 left == VarInfo::EXPONENT) {
		VarInfo info(lGrp, left);
		env.exprTypes[toString()] = info;
		return info;
	}
	// elements in the same group are okay
	else if (left == right && lGrp == rGrp && 
			 left == VarInfo::ELEMENT) {
		VarInfo info(lGrp, VarInfo::ELEMENT);
		env.exprTypes[toString()] = info;
		return info;
	}
	else {
		throw CashException(CashException::CE_PARSE_ERROR,
							"Cannot mix different types of variables");
	}
}

pair<VarInfo,VarInfo> ASTBinaryOp::getTypes(Environment &env) {	
	VarInfo leftInfo;
	VarInfo rightInfo;
	// don't recompute types if we don't have to
	if (env.exprTypes.count(lhs->toString()) == 0) 
		leftInfo = lhs->getExprType(env);
	else 
		leftInfo = env.exprTypes.at(lhs->toString());
	if (env.exprTypes.count(rhs->toString()) == 0)
		rightInfo = rhs->getExprType(env);
	else
		rightInfo = env.exprTypes.at(rhs->toString());
	return make_pair(leftInfo, rightInfo);
}

ZZ ASTAdd::eval(Environment &env) { 			
	pair<VarInfo,VarInfo> p = getTypes(env);
	VarInfo leftInfo = p.first;
	VarInfo rightInfo = p.second;
	// only need to do modular operations if one of the 
	// variables is a group element; otherwise just do things
	// over the integers
	bool isElmt = (leftInfo.type == VarInfo::ELEMENT || 
				   rightInfo.type == VarInfo::ELEMENT);
	// integers/exponents can just be added, but if there is an
	// exponent we want to retain the group information
	if (isElmt == 0){
		return lhs->eval(env) + rhs->eval(env);
	}
	else {
		const Group* lGroup = env.groups.at(leftInfo.group);
		const Group* rGroup = env.groups.at(rightInfo.group);
		const Group* retGroup = (lGroup != 0 ? lGroup : rGroup);
		assert(retGroup);
		ZZ mod = retGroup->getModulus();
		return AddMod(lhs->eval(env), rhs->eval(env), mod);
	}
}		

ZZ ASTSub::eval(Environment &env) { 			
	pair<VarInfo,VarInfo> p = getTypes(env);
	VarInfo leftInfo = p.first;
	VarInfo rightInfo = p.second;
	bool isElmt = (leftInfo.type == VarInfo::ELEMENT || 
				   rightInfo.type == VarInfo::ELEMENT);
	// integers/exponents can just be subtracted, but if there is an
	// exponent we want to retain the group information
	if (isElmt == 0){
		return lhs->eval(env) - rhs->eval(env);
	} 
	else {
		const Group* lGroup = env.groups.at(leftInfo.group);
		const Group* rGroup = env.groups.at(rightInfo.group);
		const Group* retGroup = (lGroup != 0 ? lGroup : rGroup);
		assert(retGroup);
		ZZ mod = retGroup->getModulus();
		return SubMod(lhs->eval(env), rhs->eval(env), mod);
	}
}	

ZZ ASTMul::eval(Environment &env) { 			
	pair<VarInfo,VarInfo> p = getTypes(env);
	VarInfo leftInfo = p.first;
	VarInfo rightInfo = p.second;
	bool isElmt = (leftInfo.type == VarInfo::ELEMENT || 
				   rightInfo.type == VarInfo::ELEMENT);
	// integers/exponents can just be multiplied, but if there is an
	// exponent we want to retain the group information
	if (isElmt == 0){
		return lhs->eval(env) * rhs->eval(env);
	}
	else {
		const Group* lGroup = env.groups.at(leftInfo.group);
		const Group* rGroup = env.groups.at(rightInfo.group);
		const Group* retGroup = (lGroup != 0 ? lGroup : rGroup);
		assert(retGroup);
		ZZ mod = retGroup->getModulus();
		return MulMod(lhs->eval(env), rhs->eval(env), mod);
	}
}	

ZZ ASTDiv::eval(Environment &env) { 			
	pair<VarInfo,VarInfo> p = getTypes(env);
	VarInfo leftInfo = p.first;
	VarInfo rightInfo = p.second;
	// XXX: this will cause a problem if we ever try to use
	// it during constant propagation/substitution
	const Group* lGroup = env.groups.at(leftInfo.group);
	const Group* rGroup = env.groups.at(rightInfo.group);
	const Group* retGroup = (lGroup != 0 ? lGroup : rGroup);
	// integers can just be divided
	if ((lGroup == 0 && rGroup == 0) 
		|| (leftInfo.type == VarInfo::MODULUS 
			|| rightInfo.type == VarInfo::MODULUS)) {
		return lhs->eval(env) / rhs->eval(env);
	} else if (leftInfo.type == VarInfo::EXPONENT || 
			   rightInfo.type == VarInfo::EXPONENT) {
		// want to get LHS * inv(RHS)
		// this only works if we are in a group with known order
		if (retGroup->getType() == Group::TYPE_PRIME) {
			ZZ ord = retGroup->getOrder();
			ZZ right = InvMod(rhs->eval(env), ord);
			return lhs->eval(env) * right;
			// in an RSA group, only permit computing 1/x if the caller
			// knows the order of the group
		} else if(retGroup->getType() == Group::TYPE_RSA && 
				  (retGroup->getOrder() != 0)) {
			ZZ ord = retGroup->getOrder();
			ZZ right = InvMod(rhs->eval(env), ord);
			return lhs->eval(env) * right;
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
								"That operation is not permitted in an RSA group");
		}
	} else {
		assert(retGroup);
		ZZ mod = retGroup->getModulus();
		return MulMod(lhs->eval(env), InvMod(rhs->eval(env), mod), mod);
	}
}	

ZZ ASTPow::eval(Environment &env) { 
	VarInfo info;
	if (env.exprTypes.count(lhs->toString()) == 0)
		info = lhs->getExprType(env);
	else
		info = env.exprTypes.at(lhs->toString());
	const Group* lGroup = env.groups.at(info.group);
	// if base is an exponent or integer, do it over integers
	if (lGroup == 0 || info.type == VarInfo::EXPONENT) {
		return power(lhs->eval(env), to_int(rhs->eval(env)));
	}
	else {
		return env.modPow(lhs->toString(), lhs->eval(env), rhs->eval(env), 
						  lGroup->getModulus());
	}
}

VarInfo ASTPow::getExprType(Environment &env) {
	// can raise an element to an exponent, an integer to an exponent, 
	// and an element to an integer, and an integer to an integer
	// also for square stuff can raise an exponent to an integer
	pair<VarInfo,VarInfo> p = getTypes(env);
	VarInfo leftInfo = p.first;
	VarInfo rightInfo = p.second;
	Type left = leftInfo.type;
	Type right = rightInfo.type;
	string lGrp = leftInfo.group;
	string rGrp = rightInfo.group;

	if ((left == VarInfo::ELEMENT && right == VarInfo::EXPONENT) ||
		(left == VarInfo::ELEMENT && right == VarInfo::MODULUS)){
		VarInfo info(lGrp, left);
		env.exprTypes[toString()] = info;
		return info;
	}
	else if (left == VarInfo::EXPONENT && right == VarInfo::INTEGER) {
		VarInfo info(lGrp, left);
		env.exprTypes[toString()] = info;
		return info;
	}
	else if (left == VarInfo::ELEMENT && right == VarInfo::INTEGER) {
		VarInfo info(lGrp, left);
		env.exprTypes[toString()] = info;
		return info;
	}
	else if (left == VarInfo::INTEGER && right == VarInfo::EXPONENT) {
		VarInfo info(rGrp, VarInfo::ELEMENT);
		env.exprTypes[toString()] = info;
		return info;
	}
	else if (left == right && left == VarInfo::INTEGER) {
		VarInfo info(lGrp, left);
		env.exprTypes[toString()] = info;
		return info;
	}
	else {
		throw CashException(CashException::CE_PARSE_ERROR,
							"Cannot perform exponentiation with those variables");
	}
}	
