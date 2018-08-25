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

class RulesTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract()
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContractFile(
				"test/lity/contracts/rules.sol",
				"C")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LityRules, RulesTestFramework)

BOOST_AUTO_TEST_CASE(numberOfPeople)
{
	deployContract();
	// empty list
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(0)));
}

BOOST_AUTO_TEST_CASE(isEligible)
{
	deployContract();
	// not in list, not eligible
	BOOST_REQUIRE(callContractFunction("isEligible()") == encodeArgs(false));
}

BOOST_AUTO_TEST_CASE(addPerson)
{
	deployContract();
	u256 balance = 10 * ether;
	sendEther(account(1), balance);

	// add account(1) to list
	BOOST_REQUIRE(callContractFunctionFrom(1, "addPerson(uint256)", u256(18)) == encodeArgs());
	balance -= gasCost();
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance);

	// list size is 1
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(1)));
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance);

	// in list, eligible
	BOOST_REQUIRE(callContractFunctionFrom(1, "isEligible()") == encodeArgs(true));
	balance -= gasCost();
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance);
}

BOOST_AUTO_TEST_CASE(pay)
{
	deployContract();
	u256 balance[3] = { 0, 10 * ether, 10 * ether };
	sendEther(account(1), balance[1]);
	sendEther(account(2), balance[2]);

	// add account(1) to list
	BOOST_REQUIRE(callContractFunctionFrom(1, "addPerson(uint256)", u256(18)) == encodeArgs());
	balance[1] -= gasCost();
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance[1]);

	// add account(2) to list
	BOOST_REQUIRE(callContractFunctionFrom(2, "addPerson(uint256)", u256(7)) == encodeArgs());
	balance[2] -= gasCost();
	BOOST_CHECK_EQUAL(balanceAt(account(2)), balance[2]);

	// list size is 2
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(2)));

	// in list, eligible
	BOOST_REQUIRE(callContractFunctionFrom(1, "isEligible()") == encodeArgs(true));
	balance[1] -= gasCost();
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance[1]);
	BOOST_REQUIRE(callContractFunctionFrom(2, "isEligible()") == encodeArgs(true));
	balance[2] -= gasCost();
	BOOST_CHECK_EQUAL(balanceAt(account(2)), balance[2]);

	// pay, only account(1) get paid
	sendEther(m_contractAddress, 10000);
	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs());
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance[1] + 10);
	BOOST_CHECK_EQUAL(balanceAt(account(2)), balance[2]);
}

BOOST_AUTO_TEST_CASE(factDeleteBasicTest)
{
	deployContract();
	int const numUser = 5;
	u256 balance[numUser+1];
	bool inWorkingMemory[numUser+1];
	for (int i = 1; i <= numUser; i++)
	{
		balance[i] = 1000 * ether;
		sendEther(account(i), balance[i]);
		BOOST_REQUIRE(callContractFunctionFrom(i, "addPerson(uint256)", u256(18)) == encodeArgs());
		balance[i] -= gasCost();
		BOOST_CHECK_EQUAL(balanceAt(account(i)), balance[i]);
		inWorkingMemory[i] = true;
	}

	int numPersonInWorkingMemory = numUser;

	srand(42); // TODO: Use more deterministic random number generator
	// randomly manipulate working memory
	for (int i = 0; i < 13; i++)
	{
		int u = rand()%numUser + 1;
		if (inWorkingMemory[u])
			BOOST_REQUIRE(callContractFunctionFrom(u, "deletePerson()") == encodeArgs());
		else
			BOOST_REQUIRE(callContractFunctionFrom(u, "addPerson(uint256)", u256(18)) == encodeArgs());
		balance[u] -= gasCost();
		BOOST_CHECK_EQUAL(balanceAt(account(u)), balance[u]);
		inWorkingMemory[u] = !inWorkingMemory[u];
		numPersonInWorkingMemory += inWorkingMemory[u]*2 - 1;
	}

	assert(0 < numPersonInWorkingMemory && numPersonInWorkingMemory < numUser);

	// check if number of person in working memory is correct
	// TODO: In rules.sol , numberOfPeople() do not correctly return number of people in working memory
	// BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(numPersonInWorkingMemory)));

	// pay money to persons in working memory
	sendEther(m_contractAddress, u256(10000) * numUser);
	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs());

	for (int i = 1; i <= numUser; i++)
		BOOST_CHECK_EQUAL(balanceAt(account(i)), balance[i] + 10*inWorkingMemory[i]);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
