/*==- test/lity/ContractExecutionFramework.h -------------------------------==

  This file is part of the Lity project distributed under GPL-3.0.
  See LICENSE.txt for details.

==------------------------------------------------------------------------==*/
/**
 *  @author Yi Huang <yi@skymizer.com>
 *  @date 2018
 *  Framework for executing contracts and testing them using RPC.
 */

#pragma once

#include <test/libsolidity/SolidityExecutionFramework.h>

namespace dev
{
namespace solidity
{
namespace test
{
	using Address = dev::test::Address;

class ContractExecutionFramework: public SolidityExecutionFramework
{
protected:
	// 20000000000 is a magic number.
	u256 gasCost() { return m_gasUsed * u256(20) * u256(1000000000); }

	bytes compileContractFile(
		std::string const& _sourceFile,
		std::string const& _contractName
	);

	template <class... Args>
	bytes const& callContractFunctionFrom(
		size_t _i,
		std::string _sig,
		Args const&... _arguments
	)
	{
		Address prev = m_sender;
		m_sender = account(_i);
		bytes const& ret = callContractFunction(_sig, _arguments...);
		m_sender = prev;
		return ret;
	}
};

} // end namespace test
} // end namespace solidity
} // end namespace dev
