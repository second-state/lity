pragma lity ^1.2.0;
contract C {
    struct Person {
        uint age;
        bool eligible;
    }
    struct Account {
        uint pi;
        uint bank;
        uint money;
    }

    mapping (address => uint) addr2idx;
    Person[] ps;
    Account[] acs;

    rule "bankEQ0" when {
        a: Account(bank == 0, ps[pi].eligible);
    } then {
        a.money += 10000;
        ps[a.pi].eligible = false;
    }

    rule "ageGT60" when {
        // Not implemented yet.
        // p: Person(age > 60);
        // a: Account(ps[pi].age > 60, ps[pi] != p);
        a: Account(ps[pi].age > 60);
    } then {
        a.money += 777;
    }

    function numberOfPeople() public view returns (uint) {
        return ps.length;
    }
    function numberOfAccount() public view returns (uint) {
        return acs.length;
    }
    function balance(uint i) public view returns (uint) {
        return acs[i].money;
    }
    function isEligible(uint i) public view returns (bool) {
        return ps[i].eligible;
    }
    function addPerson(uint age) public {
        ps.push(Person(age, true));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
    }
    function addAccount(uint i, uint bank) public {
        acs.push(Account(i, bank, 0));
        addr2idx[msg.sender] = factInsert acs[acs.length-1];
    }
    function pay() public {
        fireAllRules;
    }
    function () public payable { }
}
