contract C {
    struct Person {
        int age;
        bool eligible;
        address addr;
    }

    rule "myRule" when {
        p: Person(age > 10, eligible);
    } then {
        p.addr.transfer(10);
        p.eligible = false;
    }

    mapping (address => uint256) addr2idx;
    Person[] ps;

    function addPerson(int age) public {
        ps.push(Person(age, true, msg.sender));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
    }
    function deletePerson() public {
        factDelete addr2idx[msg.sender];
    }
    function pay() public {
        fireAllRules;
    }

    function () public payable { }
}
// ----
