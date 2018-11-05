#include <libsolidity/codegen/CompilerContext.h>
#include <libsolidity/codegen/DynArrUtils.h>
#include <libsolidity/codegen/CompilerUtils.h>

using namespace std;

namespace dev
{
namespace solidity
{

// Stack pre : reference
// Stack post:
void DynArrUtils::clearArray()
{
	m_context << 0 << Instruction::DUP2 << 32 << Instruction::ADD;
	// stack: refer 0 refer+32
	m_context << Instruction::MSTORE;
	// stack: refer
	m_context << 0 << Instruction::DUP2 << 64 << Instruction::ADD;
	// stack: refer 0 refer+64
	m_context << Instruction::MSTORE;
	// stack: refer
	m_context << 0 << Instruction::SWAP1;
	// stack: 0 refer
	m_context << Instruction::MSTORE;
}

// Stack pre : reference item
// Stack post:
void DynArrUtils::pushItem()
{
	int stackHeight = m_context.stackHeight();
	eth::AssemblyItem noRealloc = m_context.newTag();

	// stack: refer item
	m_context << dupInstruction(elementSize+1);
	getLen();
	// stack: refer item len
	m_context << dupInstruction(elementSize+2);
	getCap();
	// stack: refer item len cap
	m_context << Instruction::EQ;
	// stack: len==cap
	m_context << 1 << Instruction::XOR;
	// stack: len!=cap
	m_context.appendConditionalJumpTo(noRealloc);

	// stack: refer item
	m_context << dupInstruction(elementSize+1);
	getCap();
	// stack: refer item cap
	m_context << 1 << Instruction::ADD << 2 << Instruction::MUL;
	// stack: refer item (cap+1)*2
	m_context << dupInstruction(elementSize+2) << Instruction::SWAP1;
	// stack: refer item refer (cap+1)*2
	reAlloc();

	m_context << noRealloc;
	// stack: refer item
	m_context << dupInstruction(elementSize+1);
	// stack: refer item refer
	incrLen();
	// stack: refer item len'
	m_context << dupInstruction(elementSize+2) << Instruction::SWAP1;
	// stack: refer item refer len'
	m_context << 1 << Instruction::SWAP1 << Instruction::SUB;
	// stack: ... refer len'-1
	accessIndex(false);
	// stack: refer item memAddrToBePlaced
	for(int i=0; i<elementSize; i++)
	{
		m_context << Instruction::SWAP1 << Instruction::DUP2;
		// stack: refer item memAddrToBePlaced item.back(word-sized) memAddrToBePlaced
		m_context << 32*(elementSize-1-i) << Instruction::ADD << Instruction::MSTORE;
	}
	// stack: refer memAddrToBePlaced
	m_context << Instruction::POP << Instruction::POP;
	solAssert(stackHeight-(int)m_context.stackHeight() == elementSize+1, "stack height error in DynArrUtils::pushItem");
}

// Stack pre : reference
// Stack post:
void DynArrUtils::popItem()
{
	m_context << Instruction::DUP1;
	getLen();
	// stack refer len
	m_context << 1 << Instruction::SUB;
	// stack refer len'
	m_context << Instruction::DUP2 << 32 << Instruction::ADD;
	// stack refer len' refer+32
	m_context << Instruction::MSTORE;
	// stack refer
	m_context << Instruction::POP;
}

// Stack pre :
// Stack post:
void DynArrUtils::alloc()
{
	m_context << 32*3;
	utils().allocateMemory();
}

// Stack pre : reference cap'(#elmts)
// Stack post:
void DynArrUtils::reAlloc()
{
	m_context << Instruction::DUP1;
	// stack: refer cap' cap'
	m_context << Instruction::DUP3;
	// stack: ... cap' refer
	m_context << 64 << Instruction::ADD;
	// stack: ... cap' refer+64
	m_context << Instruction::MSTORE;                    // cap++
	// stack: refer cap'
	m_context << 32*elementSize << Instruction::MUL;
	// stack: refer len*32*eleSize
	utils().allocateMemory();
	// stack: refer dataPtr'
	m_context << Instruction::DUP2 << Instruction::MLOAD;
	// stack: refer dataPtr' dataPtr
	m_context << Instruction::DUP2 << Instruction::DUP4 << Instruction::MSTORE;
	m_context << Instruction::DUP3 << 32 << Instruction::ADD;
	// stack: refer dataPtr' dataPtr refer+32
	m_context << Instruction::MLOAD;
	// stack: refer dataPtr' dataPtr len
	m_context << 32*elementSize << Instruction::MUL;
	// stack: refer dataPtr' dataPtr len*32*eleSize
	m_context << Instruction::SWAP2 << Instruction::SWAP1;
	// stack: refer len*32*eleSize dataPtr' dataPtr
	utils().memoryCopy32();
	// stack: refer
	m_context << Instruction::POP;
}

// Stack pre : reference
// Stack post: len'
void DynArrUtils::incrLen()
{
	m_context << Instruction::DUP1;
	getLen();
	// stack: refer len
	m_context << 1 << Instruction::ADD;
	// stack: refer len'
	m_context << Instruction::DUP1 << Instruction::SWAP2 << 32 << Instruction::ADD;
	// stack: len' len' refer+32
	m_context << Instruction::MSTORE;
}

// Stack post: memory_pre
// Stack post: elmt
void DynArrUtils::extractElmtToStack()
{
	for(int i=0; i<elementSize; i++)
	{
		m_context << dupInstruction(1);
		m_context << 32*i << Instruction::ADD << Instruction::MLOAD;
		m_context << Instruction::SWAP1;
		// stack: ... Ei elmtMemAddr
	}
	m_context << Instruction::POP;
	// stack: E0, E1 ... En-1
}

// Stack pre: reference index
// Stack post: memory_offset
void DynArrUtils::accessIndex(bool _doBoundsCheck)
{
	if(_doBoundsCheck)
	{
		// TODO
	}
	m_context << 32*elementSize << Instruction::MUL;
	// refer index*32*eleSize
	m_context << Instruction::SWAP1 << Instruction::MLOAD;
	// 32*index*eleSize dataPtr
	m_context << Instruction::ADD;
}

/// Stack pre : reference
/// Stack post:
/// Stack state of f (if not breakable):
///   Stack pre : elmtMemAddr
///   Stack post:
/// Stack state of f (if breakable):
///   Stack pre : elmtMemAddr
///   Stack post: breakOrNot
void DynArrUtils::forEachDo(std::function<void(CompilerContext&)> f, bool breakable)
{
	int beginH = m_context.stackHeight();
	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();
	// stack: refer
	m_context << Instruction::DUP1;
	getLen();
	// stack: refer len
	m_context << 0;
	// stack: refer len i
	m_context << loopStart;                                  // loop:
	m_context << Instruction::DUP2 << Instruction::DUP2;
	// stack: ... len i
	m_context << Instruction::LT;       //   if i>=len
	m_context << 1 << Instruction::XOR;
	// stack: ... !(len>i)
	m_context.appendConditionalJumpTo(loopEnd);              //     break
	// stack: refer len i
	m_context << Instruction::DUP3 << Instruction::DUP2;
	// stack: ... refer i
	accessIndex(false);
	// stack: ... elmtMemAddr
	if (breakable)
		m_context << 0 << Instruction::SWAP1; // breakOrNot
	// stack: ... (breakOrNot) elmtMemAddr
	f(m_context);
	// stack: ... (breakOrNot)
	if (breakable)
		m_context.appendConditionalJumpTo(loopEnd);
	// stack: refer len i
	m_context << 1 << Instruction::ADD;                      //   i++
	m_context.appendJumpTo(loopStart);
	m_context << loopEnd;                                    // loopEnd:
	// stack: refer len i
	for(int i=0; i<3; i++)
		m_context << Instruction::POP;

	int endH=m_context.stackHeight();
	solAssert(beginH - endH == 1, "stack height error in DynArrUtils::forEachDo");

}

CompilerUtils DynArrUtils::utils()
{
	return CompilerUtils(m_context);
}


}
}
