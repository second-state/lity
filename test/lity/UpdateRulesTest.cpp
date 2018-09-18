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

static map<string, unique_ptr<bytes>> s_compiledContracts;

class UpdateRulesTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract(string _contractFileName)
	{
		if (!s_compiledContracts[_contractFileName])
			s_compiledContracts[_contractFileName].reset(new bytes(compileContractFile(
				"test/lity/contracts/" + _contractFileName,
				"C")));

		bytes args = encodeArgs();
		sendMessage(*(s_compiledContracts[_contractFileName]) + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LityRules, UpdateRulesTestFramework)

BOOST_AUTO_TEST_CASE(updateBasicTest)
{
	deployContract("updaterules.sol");
	int const numUser = 10;
	u256 balance[numUser+1];
	int age[numUser+1];
	srand(42);
	for (int i = 1; i <= numUser; i++)
	{
		balance[i] = 1000 * ether;
		sendEther(account(i), balance[i]);
		BOOST_REQUIRE(callContractFunctionFrom(i, "addPerson(uint256)", u256((age[i]=1+rand()%10))) == encodeArgs());
		balance[i] -= gasCost();
		BOOST_CHECK_EQUAL(balanceAt(account(i)), balance[i]);
	}

	// pay money to persons in working memory
	sendEther(m_contractAddress, u256(10000) * numUser);
	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs(true));

	for (int i = 1; i <= numUser; i++)
		BOOST_CHECK_EQUAL(balanceAt(account(i)), balance[i] + 10*age[i]);
}


BOOST_AUTO_TEST_CASE(fibonacciTest)
{
	deployContract("fibonacci.sol");

	sendEther(m_contractAddress, u256(10000));

	BOOST_REQUIRE(callContractFunction("calc()") == encodeArgs(true));

	BOOST_REQUIRE(callContractFunction("get(uint256)", u256(7)) == encodeArgs(u256(13)));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
