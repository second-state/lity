
#include <utility>
#include <numeric>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <libdevcore/Common.h>
#include <libdevcore/SHA3.h>
#include <libsolidity/ast/AST.h>
#include <libsolidity/codegen/ExpressionCompiler.h>
#include <libsolidity/codegen/RuleEngineCompiler.h>
#include <libsolidity/codegen/CompilerContext.h>
#include <libsolidity/codegen/CompilerUtils.h>
#include <libsolidity/codegen/LValue.h>
#include <libsolidity/codegen/ENIHandler.h>
#include <libevmasm/GasMeter.h>

#include <libdevcore/Whiskers.h>

using namespace std;

namespace dev
{
namespace solidity
{


// TODO: refresh the set of facts in each alpha nodes,
void RuleEngineCompiler::appendFireAllRules(ContractDefinition const& _contract)
{
	_contract.accept(*this);
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
	// stack: factID
	m_context << Instruction::DUP1;
	// stack: factID factID

	// now we try to find out the listAddress which the factID stored in
	m_context << getIdToListXorMask() << Instruction::XOR << Instruction::SLOAD;
	// stack: factID listOfThisType

	appendDeleteItemInStorageArray();
}


bool RuleEngineCompiler::visit(Rule const& _node)
{
	m_currentRule = &_node;
	return true;
}

bool RuleEngineCompiler::visit(FactDeclaration const& _node)
{
	m_currentFact = &_node;
	m_currentFieldNo = 0;
	// storage
	auto inListAddr = keccak256(_node.type()->richIdentifier()+"-factlist");
	// storage
	auto outListAddr = keccak256(m_currentRule->name()+_node.name()+"-factlist");
	m_nodeOutListAddr.push_back(outListAddr);

	m_context << 0 << outListAddr << Instruction::SSTORE; // set list as empty

	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();

	m_context << 0 << inListAddr << Instruction::SLOAD;
	// stack: i len                                          // i=0, len
	m_context << loopStart;                                  // loop:
	// stack: i len
	m_context << Instruction::DUP2 << Instruction::DUP2;
	// stack: i len i len
	m_context << Instruction::GT << 1 << Instruction::XOR;   //   if i>=len
	// stack: i len !(len>i)
	m_context.appendConditionalJumpTo(loopEnd);              //     break
	// stack: i len
	m_context << inListAddr << Instruction::DUP3;
	// stack: i len inList i
	appendAccessIndexStorage();
	// stack: i len fact
	appendPushItemToStorageArray(outListAddr);
	m_context << Instruction::POP;
	// stack: i len
	m_context << Instruction::DUP2 << 1 << Instruction::ADD;  //   i++
	// stack: i len i'
	m_context << Instruction::SWAP2 << Instruction::POP;
	// stack: i' len

	m_context.appendJumpTo(loopStart);
	m_context << loopEnd;                                    // loopEnd:

	// stack: i len
	m_context << Instruction::POP << Instruction::POP;
	return true;
}

bool RuleEngineCompiler::visit(FieldExpression const& _fieldExpr)
{
	// stack pre:
	// stack post:

	// Node function
	// input  : list of factID (in storage)
	// output : list of factID (in storage)
	// outline:
	//   get inList address by hash
	//   get outList address by hash
	//   for each fact in inList
	//     if FieldExp(the item)
	//     put this fact to outList

	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();
	eth::AssemblyItem noAddToList = m_context.newTag();
	string nodeName = m_currentRule->name()+"-"+m_currentFact->name()+"-"+to_string(m_currentFieldNo);

	m_nodeOutListAddr.push_back(keccak256(nodeName+"-factlist")); // TODO: dynamic allocation

	auto inListAddr = m_nodeOutListAddr[m_nodeOutListAddr.size()-2];
	auto outListAddr = m_nodeOutListAddr[m_nodeOutListAddr.size()-1];

	m_context << 0 << outListAddr << Instruction::SSTORE; // set list as empty
	m_context << 0;                                          // i=0
	// stack: i
	m_context << loopStart;                                  // loop:
    m_context << inListAddr << Instruction::SLOAD;           //
	// stack: i len
	m_context << Instruction::DUP2 << Instruction::LT;       //   if i>=len
	m_context << 1 << Instruction::XOR;
	// stack: i !(len>i)
	m_context.appendConditionalJumpTo(loopEnd);              //     break
	// stack: i
	m_context << inListAddr << Instruction::DUP2;
	// stack: i inList i
	appendAccessIndexStorage();
	// stack: i fact
	m_context << Instruction::DUP1;

	// save fact to a place
	// TODO: Fix this temperary(wrong) method
	m_context << 0x1234 << Instruction::SSTORE;
	                                                         //   if fieldExpr(fact)
	ExpressionCompiler(m_context).compile(_fieldExpr.expression());
	// stack: i fact fieldExpr(fact)
	m_context << 1 << Instruction::XOR;
	m_context.appendConditionalJumpTo(noAddToList);
	// stack: i fact
	m_context << Instruction::DUP1;
	// stack: i fact fact
	appendPushItemToStorageArray(outListAddr);                //     add fact to outList
	// stack: i fact len'
	m_context << Instruction::POP;
	// stack: i fact
	m_context << noAddToList;
	// stack: i fact
	m_context << Instruction::POP;
	// stack: i
	m_context << 1 << Instruction::ADD;                      //   i++
	m_context.appendJumpTo(loopStart);
	m_context << loopEnd;                                    // loopEnd:
	// stack: i
	m_context << Instruction::POP;
	return false;
}

bool RuleEngineCompiler::visit(Block const& _block)
{
	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();
	auto inListAddr = m_nodeOutListAddr.back();

	m_context << 0;
	// stack: i                                              // i=0
	m_context << loopStart;                                  // loop:
	m_context << inListAddr << Instruction::SLOAD;           //
	// stack: i len
	m_context << Instruction::DUP2 << Instruction::LT;       //   if i>=len
	m_context << 1 << Instruction::XOR;
	// stack: i !(len>i)
	m_context.appendConditionalJumpTo(loopEnd);              //     break
	// stack: i
	m_context << inListAddr << Instruction::DUP2;
	// stack: i inList i
	appendAccessIndexStorage();
	// stack: i fact

	// save fact to a place
	// TODO: Fix this temperary(wrong) method
	m_context << 0x1234 << Instruction::SSTORE;
	// stack: i
	ExpressionCompiler exprCompiler(m_context);
	_block.accept(exprCompiler);

	// pop out stack elements in this block
	// TODO: Fix this
	for(auto stmt: _block.statements())
	{
		if (auto exprStmt = dynamic_cast<ExpressionStatement const*>(stmt.get()))
			CompilerUtils(m_context).popStackElement(*(exprStmt->expression().annotation().type));
		else
			solUnimplemented("Sorry, currently only expressionStatements are allowed in then block");
	}

	// stack: i
	m_context << 1 << Instruction::ADD;                      //   i++
	m_context.appendJumpTo(loopStart);
	m_context << loopEnd;                                    // loopEnd:

	// stack: i
	m_context << Instruction::POP;
	return false;
}

void RuleEngineCompiler::endVisit(Rule const&)
{
	m_currentRule = nullptr;
}

void RuleEngineCompiler::endVisit(FactDeclaration const&)
{
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
	m_context << 1 << Instruction::XOR;
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

}
}
