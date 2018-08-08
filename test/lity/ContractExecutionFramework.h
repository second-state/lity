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

class ContractExecutionFramework: public SolidityExecutionFramework
{
protected:
	bytes compileContractFile(
		std::string const& _sourceFile,
		std::string const& _contractName
	);
};

} // end namespace test
} // end namespace solidity
} // end namespace dev
