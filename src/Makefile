CASHSOURCE =  Arbiter.cpp \
			  base64.cpp \
			  Bank.cpp \
			  BankParameters.cpp \
			  BankTool.cpp \
			  BankWithdrawTool.cpp \
			  Buyer.cpp \
			  BuyMessage.cpp \
			  CLBlindIssuer.cpp \
			  CLBlindRecipient.cpp \
			  CLSignatureProver.cpp \
			  CLSignatureVerifier.cpp \
			  CashException.cpp \
			  Ciphertext.cpp \
			  Coin.cpp \
			  CommonFunctions.cpp \
			  FEContract.cpp \
			  FEInitiator.cpp \
			  FEResponder.cpp \
			  FESetupMessage.cpp \
			  FourSquares.cpp \
			  Group.cpp \
			  GroupPrime.cpp \
			  GroupRSA.cpp \
			  GroupSquareMod.cpp \
			  Hash.cpp \
			  Merkle.cpp \
			  MerkleProof.cpp \
			  MerkleProver.cpp \
			  MerkleVerifier.cpp \
			  MultiExp.cpp \
			  ProgramMaker.cpp \
			  Seller.cpp \
			  Serialize.cpp \
			  SigmaProof.cpp \
			  SigmaProver.cpp \
			  SigmaVerifier.cpp \
			  Signature.cpp \
			  Timer.cpp \
			  UserTool.cpp \
			  UserWithdrawTool.cpp \
			  VEDecrypter.cpp \
			  VEProver.cpp \
			  VEVerifier.cpp \
			  Wallet.cpp \
			  NTL/ZZ.cpp \
			  ZKP/ASTNode.cpp \
			  ZKP/BindGroupValues.cpp \
			  ZKP/CommitmentVisitor.cpp \
			  ZKP/ComputationVisitor.cpp \
			  ZKP/ConstantProp.cpp \
			  ZKP/ConstantSub.cpp \
			  ZKP/DescribeRelations.cpp \
			  ZKP/DLRepresentation.cpp \
			  ZKP/Environment.cpp \
			  ZKP/EqualityProver.cpp \
			  ZKP/EqualityVerifier.cpp \
			  ZKP/ExponentSub.cpp \
			  ZKP/ForExpander.cpp \
			  ZKP/Interpreter.cpp \
			  ZKP/InterpreterCache.cpp \
			  ZKP/InterpreterProver.cpp \
			  ZKP/InterpreterVerifier.cpp \
			  ZKP/Printer.cpp \
			  ZKP/Translator.cpp \
			  ZKP/TypeChecker.cpp \
			  ZKP/TypeIdentifier.cpp \
			  ZKP/UndefinedVariables.cpp \
			  ZKP/UnusedVariables.cpp \
			  ZKP/ZKPLexer.cpp \
			  ZKP/ZKPParser.cpp 

OPENSSL_INC := $(shell pkg-config openssl --cflags)
OPENSSL_LIB := $(shell pkg-config openssl --libs)

CC = gcc
CPP = g++

# Extra libraries: where are ANTLR and boost installed?
OPTDIR=/pro/brownie/local
ANTLRDIR=/pro/brownie/local

#TEST_BIN = brownieTest
#TEST_SRCS = BrownieTester.cpp
TEST_BIN = test
TEST_SRCS = Test.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

ZKPTEST_BIN = zkptest
ZKPTEST_SRCS = ZKP/ZKPTest.cpp
ZKPTEST_OBJS = $(ZKPTEST_SRCS:.cpp=.o)

# XXX make this work with each of tests/*.cpp
UNITTEST_BIN = test_proofs test_serialization
UNITTEST_SRCS = tests/proofs.cpp tests/test_serialization.cpp
UNITTEST_OBJS = $(UNITTEST_SRCS:.cpp=.o)

#DEBUG_CFLAGS=-D_GLIBCXX_DEBUG -fno-inline # ANTLR segfaults with _GLIBCXX_DEBUG
DEBUG_CFLAGS=-fno-inline
LIBS=-L${OPTDIR}/lib -lgmpxx -lgmp -lm $(OPENSSL_LIB) -ldl -lboost_thread-mt -lboost_serialization-mt -lboost_iostreams-mt -lantlr
CFLAGS=-O3 -Wno-deprecated -Wall  -Wno-sequence-point -Wno-parentheses -g -I. -I${OPTDIR}/include $(OPENSSL_INC) $(DEBUG_CFLAGS)
#CFLAGS=-O0 -fno-inline -Wno-deprecated -Wall  -Wno-sequence-point -Wno-parentheses -g -I. -I${OPTDIR}/include $(OPENSSL_INC) $(DEBUG_CFLAGS)

CPP_SRCS=$(CASHSOURCE) $(TEST_SRCS) $(ZKPTEST_SRCS)
CPP_OBJS=$(CPP_SRCS:.cpp=.o)

C_SRCS=dsa.c
C_OBJS=$(C_SRCS:.c=.o)

CASHLIB_OBJS=$(CASHSOURCE:.cpp=.o) $(C_OBJS)
CASHLIB_STATIC=libcash.a

DEPDIR = .deps
df = $(DEPDIR)/$(*F)

all compile: ${TEST_BIN} ${ZKPTEST_BIN}

$(CASHLIB_STATIC): $(CASHLIB_OBJS)
	ar -rcs $@ $(CASHLIB_OBJS)

testsz: Serialize.h
	ln -fs Serialize.h Serialize.cpp
	${CPP} ${CFLAGS} -D__TESTSZ__ Serialize.cpp -o $@ ${LIBS} $(CASHLIB_STATIC)

${TEST_BIN}: $(CASHLIB_STATIC) $(TEST_OBJS)
	${CPP} $(TEST_OBJS) -o ${TEST_BIN} $(CASHLIB_STATIC) ${LIBS}

${ZKPTEST_BIN}: $(CASHLIB_STATIC) $(ZKPTEST_OBJS)
	${CPP} $(ZKPTEST_OBJS) -o ${ZKPTEST_BIN} $(CASHLIB_STATIC) ${LIBS} -lboost_program_options-mt

${UNITTEST_BIN}: $(CASHLIB_STATIC) $(UNITTEST_OBJS)
	${CPP} $(UNITTEST_OBJS) -o ${UNITTEST_BIN} $(CASHLIB_STATIC) ${LIBS} -lboost_unit_test_framework-mt

#newfile: $(CASHLIB_STATIC) newfile.o
#	${CPP} newfile.o -o newfile $(CASHLIB_STATIC) ${LIBS}

#-include $(CPP_SRCS:%.cpp=$(DEPDIR)/%.P)
-include $(DEPDIR)/*.P

parser:
	cd ZKP && ${ANTLRDIR}/bin/antlr zkp.g

Serialize.h.gch: Serialize.h
	$(CPP) $(CFLAGS) -MD -c -o $@ $<

%.o : %.cpp Serialize.h.gch
	@mkdir -p $(DEPDIR)
	$(CPP) $(CFLAGS) -MD -c -o $@ $<
	@cp $*.d $(df).P; \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P; \
	  rm -f $*.d

%.o : %.c
	@mkdir -p $(DEPDIR)
	$(CC) $(CFLAGS) -MD -c -o $@ $<
	@cp $*.d $(df).P; \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P; \
	  rm -f $*.d

clean:
	@rm -f ${TEST_BIN} ${ZKPTEST_BIN} *.o NTL/*.o ZKP/*.o *.d *.P $(CASHLIB_STATIC) $(DEPDIR)/*.P
