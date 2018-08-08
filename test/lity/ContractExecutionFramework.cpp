/*==- test/lity/ContractExecutionFramework.cpp -----------------------------==

  This file is part of the Lity project distributed under GPL-3.0.
  See LICENSE.txt for details.

==------------------------------------------------------------------------==*/
/**
 *  @author Yi Huang <yi@skymizer.com>
 *  @date 2018
 *  Framework for executing contracts and testing them using RPC.
 */
#include <libdevcore/CommonIO.h>
#include <test/lity/ContractExecutionFramework.h>

// function implementation
dev::bytes dev::solidity::test::ContractExecutionFramework::compileContractFile(
	std::string const& _sourceFile,
	std::string const& _contractName
)
{
	std::string contractCode = readFileAsString(_sourceFile);
	return compileContract(contractCode, _contractName);
}
