contract C {
    struct Person {
        int age;
    }

    rule "nameConflictRule" when {
        p: Person(age > 10);
    } then {
    }

    rule "nameConflictRule" when {
        p: Person(age < 10);
    } then {
    }
}
// ----
// DeclarationError: (145-225): Identifier already declared.
