contract C {
    struct Person {
        uint age;
        bool eligible;
        address addr;
    }

    rule "myRule" when {
        p: Person(age > 10, eligible);
    } then {
        p.addr.transfer(10);
        p.eligible = false;
    }

    mapping (address => uint) addr2idx;
    Person[] ps;

    function numberOfPeople() public view returns (uint) {
        return ps.length;
    }
    function isEligible() public view returns (bool) {
        for(uint i = 0; i < ps.length; ++i)
          if(ps[i].addr == msg.sender)
            return ps[i].eligible;
        return false;
    }
    function addPerson(uint age) public {
        ps.push(Person(age, true, msg.sender));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
    }
    // Not implemented yet.
    // function deletePerson() public {
    //     factDelete addr2idx[msg.sender];
    // }
    function pay() public {
        fireAllRules;
    }
    function () public payable { }
}
// ----
