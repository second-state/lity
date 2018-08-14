contract C {
    struct Person {
        int age;
        bool eligible;
        address addr;
    }
    Person p;
    function addPersonView(int) public view {
        factInsert p;
    }
    function addPersonPure(int) public pure {
        factInsert p;
    }
}
// ----
// Warning: (169-181): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
// TypeError: (254-255): Function declared as pure, but this expression (potentially) reads from the environment or state and thus requires "view".
// TypeError: (243-255): Function declared as pure, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
