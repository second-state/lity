#include <boost/test/unit_test.hpp>
#include <test/lity/ContractExecutionFramework.h>

using namespace std;
using namespace dev::test;

namespace dev
{
namespace solidity
{
namespace test
{

static unique_ptr<bytes> s_compiledContract;

class RecursiveRulesTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract()
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContractFile(
				"test/lity/contracts/recursiverules.sol",
				"C")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

BOOST_FIXTURE_TEST_SUITE(LityRules, RecursiveRulesTestFramework)

BOOST_AUTO_TEST_CASE(no_recursive_fireAllRules)
{
	deployContract();
	int const numUser = 5;
	u256 balance[numUser+1];
	for (int i = 1; i <= numUser; i++)
	{
		balance[i] = 1000 * ether;
		sendEther(account(i), balance[i]);
		BOOST_REQUIRE(callContractFunctionFrom(i, "addPerson(uint256)", u256(18)) == encodeArgs());
		balance[i] -= gasCost();
		BOOST_CHECK_EQUAL(balanceAt(account(i)), balance[i]);
	}

	// pay money to persons in working memory
	sendEther(m_contractAddress, u256(10000) * numUser);
	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs(true));
}

BOOST_AUTO_TEST_CASE(recursive_fireAllRules)
{
	deployContract();
	int const numUser = 5;
	u256 balance[numUser+1];
	for (int i = 1; i <= numUser; i++)
	{
		balance[i] = 1000 * ether;
		sendEther(account(i), balance[i]);
		BOOST_REQUIRE(callContractFunctionFrom(i, "addPerson(uint256)", u256(36)) == encodeArgs());
		balance[i] -= gasCost();
		BOOST_CHECK_EQUAL(balanceAt(account(i)), balance[i]);
	}

	// pay money to persons in working memory
	sendEther(m_contractAddress, u256(10000) * numUser);

	// Should fail because users are too old, trigger recursive fireAllRules
	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs());
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
