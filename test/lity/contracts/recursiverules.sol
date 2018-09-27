pragma lity ^1.2.0;
contract C {
    struct Person {
        uint age;
        bool eligible;
        address addr;
    }

    rule "ageGT10newbie" when {
        p: Person(age > 10, eligible);
    } then {
        recursiveFire(p);
    }

    function recursiveFire(Person p) internal {
        if (p.age > 30)
            fireAllRules;
    }

    mapping (address => uint) addr2idx;
    Person[] ps;

    function addPerson(uint age) public {
        ps.push(Person(age, true, msg.sender));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
    }
    function deletePerson() public {
        factDelete addr2idx[msg.sender];
        delete addr2idx[msg.sender];
    }
    function pay() public returns (bool) {
        fireAllRules;
        return true;
    }
    function () public payable { }
}
