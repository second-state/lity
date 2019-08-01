contract C {
    struct Person {
        uint age;
        address addr;
    }

    rule "updateTest" when {
        p: Person(age > 0);
    } then {
        address(uint160(p.addr)).transfer(10);
        p.age--;
        update p;
    }

    mapping (address => uint) addr2idx;
    Person[] ps;

    function addPerson(uint age) public {
        ps.push(Person(age, msg.sender));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
    }
    function pay() public returns (bool) {
        fireAllRules;
        return true;
    }
    function () external payable { }
}
