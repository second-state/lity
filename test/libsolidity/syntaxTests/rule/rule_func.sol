contract C {
    struct Person {
        int age;
        bool eligible;
        address addr;
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
}
// ----
