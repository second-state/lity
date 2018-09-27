pragma lity ^1.2.0;
contract C {
    struct Person {
        uint age;
        bool eligible;
    }

    rule "ageLT18newbie" when {
        p: Person(age < 18, eligible);
    } then {
        p.eligible = false;
    }

    mapping (address => uint) addr2idx;
    Person[] ps;

    function numberOfPeople() public view returns (uint) {
        return ps.length;
    }
    function isEligible(uint i) public view returns (bool) {
        return ps[i].eligible;
    }
    function addPerson(uint age) public {
        ps.push(Person(age, true));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
    }
    function checkPeople() public {
        fireAllRules;
    }
}
