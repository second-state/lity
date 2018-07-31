contract C {
    struct Person {
        int age;
        bool eligible;
        address addr;
    }
    function addPerson(int age) public view {
        factInsert Person(age, false, address(0));
    }
}
// ----
// Warning: (155-196): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
