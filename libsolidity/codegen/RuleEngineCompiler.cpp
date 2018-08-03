
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

void RuleEngineCompiler::appendFireAllRules(ContractDefinition const& _contract)
{
	// TODO: implementation
	// refresh the set of facts in each alpha nodes,
	// actually fire the rules for each fact survived(?) in each terminal nodes, and so on.
	m_currentFieldNo = 0;
	m_nodeOutListAddr.push_back(keccak256("outlist-"+to_string(m_currentFieldNo++)));
	_contract.accept(*this);
}

void RuleEngineCompiler::appendFactInsert(TypePointer const& factType)
{
	// TODO: implementation
	// Be careful, we want to handle both memory and storage data location for the fact on stack top.
	
	h256 listAddr = keccak256(factType->richIdentifier()+"-factlist");
	
    m_context << listAddr;
	{   // append Item to storage list
        // stack: argValue ArrayReference
        shared_ptr<ArrayType> arrayType = make_shared<ArrayType>(DataLocation::Storage, factType);
        TypePointer const& argType = factType;

        m_context << Instruction::DUP1;
        ArrayUtils(m_context).incrementDynamicArraySize(*arrayType);
        // stack: argValue ArrayReference newLength
        m_context << Instruction::SWAP1;
        // stack: argValue newLength ArrayReference
        m_context << u256(1) << Instruction::DUP3 << Instruction::SUB;
        // stack: argValue newLength ArrayReference (newLength-1)
        ArrayUtils(m_context).accessIndex(*arrayType, false);
        // stack: argValue newLength storageSlot slotOffset
        utils().moveToStackTop(3, argType->sizeOnStack());
        // stack: newLength storageSlot slotOffset argValue
        utils().moveToStackTop(1 + factType->sizeOnStack());
        utils().moveToStackTop(1 + factType->sizeOnStack());
        // stack: newLength argValue storageSlot slotOffset
        StorageItem(m_context, *factType).storeValue(*factType, m_currentFact->location(), true);
		// stack: newLength
	}
	m_context << listAddr << Instruction::ADD;
	
	// stack: factAddrInList

	// register factID

	h256 IDToAddrTable = keccak256("IDToAddrTable");
    m_context << IDToAddrTable;
	{   // append Item to storage list
        shared_ptr<ArrayType> arrayType = make_shared<ArrayType>(DataLocation::Storage, factType);
        TypePointer const& argType = factType;
        // stack: argValue ArrayReference
        m_context << Instruction::DUP1;
        ArrayUtils(m_context).incrementDynamicArraySize(*arrayType);
        // stack: argValue ArrayReference newLength
        m_context << Instruction::SWAP1;
        // stack: argValue newLength ArrayReference
        m_context << u256(1) << Instruction::DUP3 << Instruction::SUB;
        // stack: argValue newLength ArrayReference (newLength-1)
        ArrayUtils(m_context).accessIndex(*arrayType, false);
        // stack: argValue newLength storageSlot slotOffset
        utils().moveToStackTop(3, argType->sizeOnStack());
        // stack: newLength storageSlot slotOffset argValue
        utils().moveToStackTop(1 + factType->sizeOnStack());
        utils().moveToStackTop(1 + factType->sizeOnStack());
        // stack: newLength argValue storageSlot slotOffset
        StorageItem(m_context, *factType).storeValue(*factType, m_currentFact->location(), true);
		// stack: newLength
	}

	m_context << 1 << Instruction::SUB;
}

void RuleEngineCompiler::appendFactDelete()
{
	// TODO: implementation
	// 1. find out the factID's type.
	// 2. remove the fact from working memory.
	m_context << Instruction::POP; // stub
}


bool RuleEngineCompiler::visit(Rule const& _node)
{
	m_currentRule = &_node;
	return true;
}

bool RuleEngineCompiler::visit(FactDeclaration const& _node)
{
	m_currentFact = &_node;
	return true;
}

bool RuleEngineCompiler::visit(FieldExpression const&)
{
	// input: list of factID (in memory)
	// output: list of factID (in memory)
	
	// stack pre:
	// stack post:

	// Outline:
	//   get input list address by hash
	//   get output list address by hash
	//   for each factID in input list
	//     extract the item
	//     if FieldExp(the item)
	//     put this factID to output

	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();
	eth::AssemblyItem noAddToList = m_context.newTag();
	string nodeName = m_currentRule->name()+"-"+m_currentFact->name()+"-"+to_string(m_currentFieldNo);

	m_currentFieldNo++;
	m_nodeOutListAddr.push_back(keccak256("outlist-"+to_string(m_currentFieldNo)));
	auto inListAddr = m_nodeOutListAddr[m_nodeOutListAddr.size()-2];
	auto outListAddr = m_nodeOutListAddr[m_nodeOutListAddr.size()-1];

	m_context << inListAddr << 32 << Instruction::ADD << inListAddr << Instruction::MLOAD;
	// stack: inputIter inputListLen
	m_context << loopStart;                                  // loop:
	m_context << Instruction::ISZERO;                        //   if(remainLen==0) 
	m_context.appendConditionalJumpTo(loopEnd);              //     break;
	m_context << 1 << Instruction::SUB;                      //   else len--;
	m_context << Instruction::DUP2 << Instruction::MLOAD;    //   mload item (factID)
	// stack: inputIter inputListRemainLen factID
	
	// TODO: load factItemAddr from IDToAddr
	// stack pre: ... factID
	// stack post: ... factID factItemAddr
	// ===============================
	// ===============================


	// TODO: check fieldExpr(factItem)                       //   if(fieldExpr(item))
	// stack: ... factItemAddr
	// ===============================
	// Check nothing. This should be replaced.
	m_context << Instruction::POP << 0;
	m_context.appendConditionalJumpTo(noAddToList);
	// =============================== 
	

    // start{push item into output}                          //      mstore item
	m_context << outListAddr << Instruction::MLOAD;
	// stack: ... factID outListLen
	m_context << 1 << Instruction::ADD << outListAddr;       // outLen++
	// stack: ... factID outListLen' outListAddr
	m_context << Instruction::DUP2 << Instruction::DUP2;
	// stack: ... factID outListLen' outListAddr outListLen' outListAddr
	m_context << Instruction::MSTORE;
	m_context << Instruction::SWAP1 << 32 << Instruction::MUL;
	m_context << Instruction::ADD;                           // OutAddrToBePlaced = outLen+outListAddr
	// stack: ... factID outAddrToBePlaced
	m_context << Instruction::MSTORE;                        // store
    // end{push item into output}                            //      mstore item

	m_context << noAddToList;
	// stack: inputIter inputListRemainLen
	m_context << Instruction::DUP2 << 1 << Instruction::ADD; //   iter++;
	// stack: inputIter inputListRemainLen inputIter+1
	m_context << Instruction::SWAP2 << Instruction::POP;     //
	// stack: inputIter' inputListRemainLen
	m_context << loopEnd;                                    // loopEnd:
	
	return false;
}

void RuleEngineCompiler::endVisit(Rule const&)
{
	m_currentRule = nullptr;
}

void RuleEngineCompiler::endVisit(FactDeclaration const&)
{
	m_currentFact = nullptr;
}

void RuleEngineCompiler::endVisit(FieldExpression const&)
{
	m_currentFieldNo = 0;
}

CompilerUtils RuleEngineCompiler::utils()
{
	return CompilerUtils(m_context);
}

}
}
