
#include <utility>
#include <numeric>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <libdevcore/Common.h>
#include <libdevcore/Keccak256.h>
#include <libsolidity/ast/AST.h>
#include <libsolidity/codegen/ExpressionCompiler.h>
#include <libsolidity/codegen/RuleEngineCompiler.h>
#include <libsolidity/codegen/CompilerContext.h>
#include <libsolidity/codegen/CompilerUtils.h>
#include <libsolidity/codegen/LValue.h>
#include <libsolidity/codegen/ENIHandler.h>
#include <libsolidity/codegen/DynArrUtils.h>
#include <libevmasm/GasMeter.h>

#include <libdevcore/Whiskers.h>

using namespace std;

namespace dev
{
namespace solidity
{
dev::u256 ReteNode::serialNo=keccak256("rule serial no base");

void RuleEngineCompiler::appendFireAllRules(ContractDefinition const& _contract)
{
	RuleEngineCompiler::appendLockRuleEngineOrFail();
	m_context.appendJumpToAndReturn(m_context.entryAllRules(_contract));
	RuleEngineCompiler::appendUnlockRuleEngine();
}

void RuleEngineCompiler::appendFactInsert(TypePointer const& _factType)
{
	// stack pre:  itemAddr
	// stack post: factID
	// Note that in current implementation, factID = itemAddr

	// Only struct types with storage data location can be inserted
	solAssert(
		dynamic_pointer_cast<StructType const>(_factType) &&
		dynamic_pointer_cast<ReferenceType const>(_factType)->location() == DataLocation::Storage
		, "Invalid factInsert operand type"
	);

	appendAssertNoRuleEngineLock(); // fail if we are inside fireAllRules

	h256 listOfThisType = keccak256(_factType->richIdentifier()+"_ptr-factlist"); // TODO: fix type name issue

	m_context << Instruction::DUP1;
	// stack: itemAddr itemAddr

	// Save the mapping which maps "itemAddr(aka factID)" to listOfThisType
	m_context << getIdToListXorMask() << Instruction::XOR;
	// stack: itemAddr (itemAddr^idToListXorMask)
	m_context << listOfThisType << Instruction::SWAP1 << Instruction::SSTORE;

	// stack: itemAddr
	m_context << Instruction::DUP1;
	// stack: itemAddr itemAddr
	appendPushItemToStorageArray(listOfThisType);
	// stack: itemAddr listLen'
	m_context << Instruction::POP;
}

void RuleEngineCompiler::appendFactDelete()
{
	appendAssertNoRuleEngineLock(); // fail if we are inside fireAllRules

	// stack: factID
	m_context << Instruction::DUP1;
	// stack: factID factID

	// now we try to find out the listAddress which the factID stored in
	m_context << getIdToListXorMask() << Instruction::XOR << Instruction::SLOAD;
	// stack: factID listOfThisType

	appendDeleteItemInStorageArray();
}

eth::AssemblyItem RuleEngineCompiler::compileNetwork(Rule const& _rule)
{
	eth::AssemblyItem ruleTag = m_context.newTag();
	m_context.appendJumpTo(ruleTag);
	// gen network nodes
	_rule.accept(*this);
	// gen node code
	for(auto pr: m_alphaNodes) compile(*pr.second);
	for(auto pr: m_typeNodes) compile(*pr.second);
	for(auto pr: m_joinNodes) compile(*pr.second);
	compile(*m_termNode);
	// controll flow of nodes
	m_context << ruleTag;
	for(auto node: m_ReteNodeOrder)
	{
		eth::AssemblyItem returnLabel = m_context.pushNewTag();
		m_context.appendJumpTo(entryNode(*node));
		m_context << returnLabel;
		m_context.adjustStackOffset(-1);
	}
	return ruleTag;
}

void RuleEngineCompiler::compile(TypeNode const& _node)
{
	m_context << entryNode((ReteNode const&)_node);
	// storage list
	auto inListAddr = keccak256(_node.type()->richIdentifier()+"-factlist");
	// listPtr(to memList) in storage
	auto outListPtrAddr = _node.outAddr();

	DynArrUtils(m_context, 1).alloc();
	m_context << outListPtrAddr << Instruction::SSTORE;

	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();

	m_context << 0 << inListAddr << Instruction::SLOAD;
	// stack: i len                                          // i=0, len
	m_context << loopStart;                                  // loop:
	// stack: i len
	m_context << Instruction::DUP2 << Instruction::DUP2;
	// stack: i len i len
	m_context << Instruction::GT << Instruction::ISZERO;     //   if i>=len
	// stack: i len !(len>i)
	m_context.appendConditionalJumpTo(loopEnd);              //     break
	// stack: i len
	m_context << inListAddr << Instruction::DUP3;
	// stack: i len inList i
	appendAccessIndexStorage();
	// stack: i len fact
	m_context << outListPtrAddr << Instruction::SLOAD;
	m_context << Instruction::SWAP1;
	// stack: listMemAddr fact
	DynArrUtils(m_context, 1).pushItem();
	// stack: i len
	m_context << Instruction::DUP2 << 1 << Instruction::ADD;  //   i++
	// stack: i len i'
	m_context << Instruction::SWAP2 << Instruction::POP;
	// stack: i' len

	m_context.appendJumpTo(loopStart);
	m_context << loopEnd;                                    // loopEnd:

	// stack: i len
	m_context << Instruction::POP << Instruction::POP;
	m_context.appendJump(eth::AssemblyItem::JumpType::OutOfFunction);
	m_context.setStackOffset(0); // not sure this is the right place
}

void RuleEngineCompiler::compile(AlphaNode const& _node)
{
	m_context << entryNode((ReteNode const&)_node);
	// listPtr(to memList) in storage
	auto inListPtrAddr  = _node.parent()->outAddr();
	auto outListPtrAddr = _node.outAddr();

	DynArrUtils(m_context, 1).alloc();
	m_context << outListPtrAddr << Instruction::SSTORE;

	m_context << inListPtrAddr << Instruction::SLOAD;
	DynArrUtils(m_context, 1).forEachDo(
		[&] (CompilerContext& context) {
			eth::AssemblyItem noAdd = context.newTag();
			// stack: elmtMemAddr
			// tupleSize=1 since it's alphaNode
			context << Instruction::MLOAD;
			// stack: fact
			context.addFact(_node.fact(), 1);
			auto exprs = _node.exprs();
			if(exprs.size()==0)
				context << 1;
			else
			{
				for(auto fieldExpr: exprs)
					ExpressionCompiler(context).compile(fieldExpr->expression());
				for(int i=0; i< int(exprs.size())-1; i++)
					m_context << Instruction::AND;
			}
			context.removeFact(_node.fact());
			// stack: fact Expr
			context << Instruction::ISZERO;
			context.appendConditionalJumpTo(noAdd);
			// stack: fact
			context << outListPtrAddr << Instruction::SLOAD;
			// stack: fact outListMemAddr
			context << Instruction::DUP2;
			// stack: fact outListMemAddr fact
			DynArrUtils(context, 1).pushItem();
			context << noAdd;
			// stack: fact
			context << Instruction::POP;
		}
	);
	m_context.appendJump(eth::AssemblyItem::JumpType::OutOfFunction);
	m_context.setStackOffset(0); // not sure this is the right place
}

void RuleEngineCompiler::compile(JoinNode const& _node)
{
	m_context << entryNode((ReteNode const&)_node);

	if(_node.leftParent()==nullptr) // alias
	{
		m_context << _node.rightParent()->outAddr() << Instruction::SLOAD;
		m_context << _node.outAddr() << Instruction::SSTORE;
	}
	else // join
	{
		JoinNode const& left = *_node.leftParent();
		AlphaNode const& right = *_node.rightParent();

		DynArrUtils(m_context, 1).alloc();
		m_context << _node.outAddr() << Instruction::SSTORE;

		m_context << right.outAddr() << Instruction::SLOAD;
		DynArrUtils(m_context, 1).forEachDo(
			[&] (CompilerContext& context)
			{
				// stack: elmtMemAddr
				// tupleSize=1 since it's alphaNode
				context << Instruction::MLOAD;
				// stack: fact
				context.addFact(right.fact(), 1);
				context << left.outAddr() << Instruction::SLOAD;

				DynArrUtils(m_context, left.tupeSize()).forEachDo(
					[&] (CompilerContext& context)
					{
						eth::AssemblyItem noAdd = context.newTag();
						// stack: elmtMemAddr
						DynArrUtils(m_context, left.tupeSize()).extractElmtToStack();
						for(int i=0; i<left.tupeSize(); i++)
							context.addFact(left.fact(i), left.tupeSize()-i);
						// stack: left.facts
						auto exprs = _node.exprs();
						if(exprs.size()==0)
							context << 1;
						else
						{
							for(auto fieldExpr: exprs)
								ExpressionCompiler(context).compile(fieldExpr->expression());
							for(int i=0; i< int(exprs.size())-1; i++)
								context << Instruction::AND;
						}
						// stack: left.facts Expr
						context << Instruction::ISZERO;
						context.appendConditionalJumpTo(noAdd);
						// stack: left.facts
						context << _node.outAddr() << Instruction::SLOAD;
						// stack: left.facts outList
						for(int i=0; i<left.tupeSize(); i++) appendFact(left.fact(i));
						appendFact(right.fact());
						// stack: left.facts outList left.facts right.fact
						DynArrUtils(m_context, _node.tupeSize()).pushItem();

						context << noAdd;
						// stack: left.facts
						for(int i=0; i<left.tupeSize(); i++)
						{
							context.removeFact(left.fact(i));
							context << Instruction::POP;
						}
					}
				);
				context.removeFact(right.fact());
				context << Instruction::POP;
			}
		);
	}
	m_context.appendJump(eth::AssemblyItem::JumpType::OutOfFunction);
	m_context.setStackOffset(0); // not sure this is the right place
}

void RuleEngineCompiler::compile(TermNode const& _node)
{
	m_context << entryNode((ReteNode const&)_node);
	// alias
	m_context << _node.parent()->outAddr() << Instruction::SLOAD;
	m_context << _node.outAddr() << Instruction::SSTORE;
	m_context.appendJump(eth::AssemblyItem::JumpType::OutOfFunction);
	m_context.setStackOffset(0); // not sure this is the right place
}

bool RuleEngineCompiler::visit(Rule const& _rule)
{
	m_currentRule = &_rule;
	return true;
}

bool RuleEngineCompiler::visit(FactDeclaration const& _fact)
{
	m_currentFact = &_fact;
	m_currentFieldNo = 0;
	return true;
}

bool RuleEngineCompiler::visit(FieldExpression const& _fieldExpr)
{
	ExprUtils exprUiils((Expression const&) _fieldExpr);
	if (exprUiils.isAlpha())
		m_alphaExprs[m_currentFact].push_back(&_fieldExpr);
	else
		m_betaExprs[m_currentFact].push_back(&_fieldExpr);
	return false;
}

void RuleEngineCompiler::endVisit(Rule const& _rule)
{
	TermNode& termNode = this->termNode();
	termNode.setPar(this->joinNode(_rule.lastFact()));

	m_currentRule = nullptr;
	m_termNode = &termNode;

	genOrder();
}

void RuleEngineCompiler::endVisit(FactDeclaration const& _fact)
{
	auto factType = _fact.type();
	TypeNode const& typeNode = this->typeNode(factType);
	AlphaNode& alphaNode = this->alphaNode(_fact);
	for (auto fieldExpr: m_alphaExprs[m_currentFact])
		alphaNode.addExpr(*fieldExpr);
	alphaNode.setPar(typeNode);

	JoinNode& joinNode = this->joinNode(_fact);
	for (auto fieldExpr: m_betaExprs[m_currentFact])
		joinNode.addExpr(*fieldExpr);
	int factIdx = m_currentRule->factIndex(_fact);
	if(factIdx!=0)
		joinNode.setLeftPar(this->joinNode(m_currentRule->fact(factIdx-1)));
	for(int i=0; i<=factIdx; i++)
		joinNode.addFact(m_currentRule->fact(i));
	joinNode.setRightPar(alphaNode);

	m_currentFact = nullptr;
	m_currentFieldNo = 0;
}

void RuleEngineCompiler::endVisit(FieldExpression const&)
{
	m_currentFieldNo++;
}

CompilerUtils RuleEngineCompiler::utils()
{
	return CompilerUtils(m_context);
}

void RuleEngineCompiler::appendFact(FactDeclaration const & _fact)
{
	auto baseStackOffset = m_context.baseStackOffsetOfFact(_fact);
	unsigned stackPos = m_context.baseToCurrentStackOffset(baseStackOffset);
	m_context << dupInstruction(stackPos + 1);
}

// push item to storage array (WARNING: this is not solidity dynamic array)
// listAddr is a compile-time known address
// stack: item
// stack: len'
// TODO: Take _listAddr from stack instead of from function parameter. (For more flexibility)
void RuleEngineCompiler::appendPushItemToStorageArray(h256 _listAddr)
{
	// stack: itemAddr
	m_context << _listAddr << _listAddr << Instruction::SLOAD;
	// stack: itemAddr _listAddr listLen
	m_context << 1 << Instruction::ADD;
	// stack: itemAddr _listAddr listLen'
	m_context << Instruction::DUP1 << Instruction::SWAP2;
	// stack: itemAddr listLen' listLen' _listAddr
	m_context << Instruction::SSTORE;                                 // store len
	// stack: itemAddr listLen'
	m_context << Instruction::SWAP1 << Instruction::DUP2;
	// stack: listLen' itemAddr listLen'
	m_context << _listAddr << Instruction::ADD << Instruction::SSTORE; // store item
	// stack: listLen'
}

// remove an item in storage array, by using O(N) linear search to find the item,
// then move the last item in the array to the deleted location, then decrease the array length.
// If can not find the element, call invalid instruction
// stack pre: itemValue listAddr
// stack post:
void RuleEngineCompiler::appendDeleteItemInStorageArray()
{
	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();
	eth::AssemblyItem elementNotFound = m_context.newTag();

	m_context << 0;
	// stack: itemValue listAddr i                           // i=0
	m_context << loopStart;                                  // loop:
	m_context << Instruction::DUP2 << Instruction::SLOAD;

	// stack: itemValue listAddr i len
	m_context << Instruction::DUP2 << Instruction::LT;
	// stack: itemValue listAddr i (i < len)
	m_context << Instruction::ISZERO;
	// stack: itemValue listAddr i (i >= len)
	m_context.appendConditionalJumpTo(elementNotFound);
	// stack: itemValue listAddr i
	m_context << Instruction::DUP2 << Instruction::DUP2;
	// stack: itemValue listAddr i listAddr i
	appendAccessIndexStorage();
	// stack: itemValue listAddr i fact
	m_context << Instruction::DUP4 << Instruction::EQ;
	// stack: itemValue listAddr i (fact == itemValue)
	m_context.appendConditionalJumpTo(loopEnd);
	// stack: itemValue listAddr i

	m_context << 1 << Instruction::ADD;                      //   i++
	m_context.appendJumpTo(loopStart);

	m_context << elementNotFound;                            // elementNotFound:
	m_context << Instruction::INVALID;

	m_context << loopEnd;                                    // loopEnd:
	// stack: itemValue listAddr i
	m_context << Instruction::DUP2 << Instruction::SLOAD;    // TODO: Avoid SLOAD(listAddr) twice
	// stack: itemValue listAddr i len
	m_context << 1 << Instruction::SWAP1 << Instruction::SUB;
	// stack: itemValue listAddr i (len-1)
	m_context << Instruction::DUP3 << Instruction::SWAP1;
	// stack: itemValue listAddr i listAddr (len-1)
	appendAccessIndexStorage();
	// stack: itemValue listAddr i fact
	m_context << Instruction::DUP3 << Instruction::DUP3 << Instruction::DUP3;
	appendWriteIndexStorage();
	// stack: itemValue listAddr i fact
	m_context << Instruction::POP << Instruction::POP;

	// Decrease length
	// stack: itemValue listAddr
	m_context << Instruction::DUP1 << Instruction::SLOAD;
	// stack: itemValue listAddr len
	m_context << 1 << Instruction::SWAP1 << Instruction::SUB;
	// stack: itemValue listAddr (len-1)
	m_context << Instruction::SWAP1 << Instruction::SSTORE;
	// stack: itemValue
	m_context << Instruction::POP;
}

void RuleEngineCompiler::appendUpdate()
{
	// execute Instruction::INVALID if the rule engine is not executing
	appendAssertHaveRuleEngineLock();
	// In current implementation we don't care which fact is updated, so we just pop it
	m_context << Instruction::POP;
	// set reevaluation mark to true
	m_context << 1 << getRuleEngineReevaluateLocation() << Instruction::SSTORE;
}

// stack pre: array index
// stack post: item
void RuleEngineCompiler::appendAccessIndexStorage()
{
	// stack: array index
	m_context << 1 << Instruction::ADD;
	// stack: array index+1
	m_context << Instruction::ADD << Instruction::SLOAD;
	// stack: item
}

// stack pre: listAddr index value
// stack post:
void RuleEngineCompiler::appendWriteIndexStorage()
{
	m_context << Instruction::DUP2 << 1 << Instruction::ADD;
	// stack: listAddr index value (index+1)
	m_context << Instruction::DUP4 << Instruction::ADD;
	// stack: listAddr index value (listAddr+index+1)
	m_context << Instruction::SSTORE << Instruction::POP << Instruction::POP;
}

void RuleEngineCompiler::appendLockRuleEngineOrFail()
{
	m_context << getRuleEngineLockLocation() << Instruction::SLOAD;
	// stack: isLocked
	m_context << Instruction::ISZERO;
	// stack: !isLocked
	eth::AssemblyItem setLock = m_context.newTag();
	m_context.appendConditionalJumpTo(setLock);
	m_context << Instruction::INVALID; // already locked
	m_context << setLock;
	m_context << 1 << getRuleEngineLockLocation() << Instruction::SSTORE;
}

void RuleEngineCompiler::appendUnlockRuleEngine()
{
	m_context << 0 << getRuleEngineLockLocation() << Instruction::SSTORE;
}

void RuleEngineCompiler::appendAssertNoRuleEngineLock()
{
	m_context << getRuleEngineLockLocation() << Instruction::SLOAD;
	// stack: isLocked
	m_context << Instruction::ISZERO;
	// stack: !isLocked
	eth::AssemblyItem ok = m_context.newTag();
	m_context.appendConditionalJumpTo(ok);
	m_context << Instruction::INVALID; // already locked
	m_context << ok;
}

void RuleEngineCompiler::appendAssertHaveRuleEngineLock()
{
	m_context << getRuleEngineLockLocation() << Instruction::SLOAD;
	// stack: isLocked
	eth::AssemblyItem ok = m_context.newTag();
	m_context.appendConditionalJumpTo(ok);
	m_context << Instruction::INVALID; // no lock
	m_context << ok;
}

void RuleEngineCompiler::appendCleanUpNodes()
{
	for (auto ptrAddr: getAllNodeOutListPtrAddr())
	{
		m_context << ptrAddr << Instruction::SLOAD;
		DynArrUtils(m_context, 1).clearArray();
	}
}

}
}
