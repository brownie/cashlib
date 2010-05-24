#include "ConstantSub.h"

void ConstantSub::applyASTForExpr(ASTForExprPtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getLBound()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setLBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getLBound()->visit(*this);
	}
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getUBound()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setUBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getUBound()->visit(*this);
	}
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getExpr()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setExpr(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getExpr()->visit(*this);
	}
}

void ConstantSub::applyASTForRel(ASTForRelPtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getLBound()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setLBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getLBound()->visit(*this);
	}
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getUBound()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setUBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getUBound()->visit(*this);
	}
	n->getRelation()->visit(*this);
}

void ConstantSub::applyASTIdentifierSub(ASTIdentifierSubPtr n){
	if(inputs.count(n->getSub())>0){
		n->setSub(lexical_cast<string>(inputs.at(n->getSub())));
	}
}

void ConstantSub::applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n){
	if(inputs.count(n->getSub())>0){
		n->setSub(lexical_cast<string>(inputs.at(n->getSub())));
	}
}

void ConstantSub::applyASTDeclIDRange(ASTDeclIDRangePtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getLP()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setLBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getLP()->visit(*this);
	}
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getUP()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setUBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getUP()->visit(*this);
	}
}

void ConstantSub::applyASTRange(ASTRangePtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getLower()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setLower(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getLower()->visit(*this);
	}
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getCenter()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setCenter(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getCenter()->visit(*this);
	}
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getUpper()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setUpper(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getUpper()->visit(*this);
	}

}


void ConstantSub::applyASTCommitment(ASTCommitmentPtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getExpr()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setExpr(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getExpr()->visit(*this);
	}
}

void ConstantSub::applyASTEqual(ASTEqualPtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getExpr()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setExpr(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getExpr()->visit(*this);
	}
}

void ConstantSub::applyASTDeclEqual(ASTDeclEqualPtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getExpr()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setExpr(repw);
			inputs[n->getId()->getName()] = repw->getVal();
			newConstant = true;
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getExpr()->visit(*this);
	}
}


void ConstantSub::applyASTBinaryOp(ASTBinaryOpPtr n){
	ASTExprIdentifierPtr isEId;
	if ((isEId = dynamic_pointer_cast<ASTExprIdentifier>(n->getLHS()))){
		if(inputs.count(isEId->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isEId->getName()));
			n->setLHS(repw);
			newConstant = true;
		} else {
			isEId->visit(*this);
		}
	} else {
		n->getLHS()->visit(*this);
	}
	if ((isEId = dynamic_pointer_cast<ASTExprIdentifier>(n->getRHS()))){
		if(inputs.count(isEId->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isEId->getName()));
			n->setRHS(repw);
			newConstant = true;
		} else {
			isEId->visit(*this);
		}
	} else {
		n->getRHS()->visit(*this);
	}
}

void ConstantSub::applyASTUnaryOp(ASTUnaryOpPtr n){
	ASTExprIdentifierPtr isEId;
	if ((isEId = dynamic_pointer_cast<ASTExprIdentifier>(n->getExpr()))){
		if(inputs.count(isEId->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isEId->getName()));
			n->setExpr(repw);
		} else {
			isEId->visit(*this);
		}
	} else {
		n->getExpr()->visit(*this);
	}
}

void ConstantSub::applyASTRandomBnd(ASTRandomBndPtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getLBound()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setLBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getLBound()->visit(*this);
	}
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getUBound()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setUBound(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getUBound()->visit(*this);
	}
}

void ConstantSub::applyASTRandomPrime(ASTRandomPrimePtr n){
	ASTExprIdentifierPtr isIdSub;
	if ((isIdSub = dynamic_pointer_cast<ASTExprIdentifier>(n->getLength()))){
		if(inputs.count(isIdSub->getName())>0){
			ASTExprIntPtr repw = new_ptr<ASTExprInt>(inputs.at(isIdSub->getName()));
			n->setLength(repw);
		} else {
			isIdSub->visit(*this);
		}
	} else {
		n->getLength()->visit(*this);
	}
}

