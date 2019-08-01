
// if you changed this file, you should also consider update
// the corresponding example in docs/rule-examples.rst

contract C {

    // "(,,◕　⋏　◕,,)
    struct Cat {
        uint256 id;
        uint256 energy;
    }

    struct CatLocation {
        uint256 id;
        uint256 value;
    }

    struct Food {
        uint256 location;
        uint256 energy;
        bool eaten;
    }

    // Note that rules appear first have higher priority,
    // so cats won't go through a food without eating it.
    rule "catEatFood"
    when {
        c1: Cat();
        cl1: CatLocation(id == c1.id);
        f1: Food(location == cl1.value, !eaten);
    } then {
        c1.energy += f1.energy;
        update c1;
        f1.eaten = true;
        update f1;
    }

    rule "catMoves"
    when {
        c1: Cat(energy > 0);
        cl1: CatLocation(id == c1.id);
    } then {
        c1.energy--;
        update c1;
        cl1.value++;
        update cl1;
    }

    Cat[] cats;
    CatLocation[] catLocations;
    uint256[] factIDs;
    Food[] foods;

    function addCat(uint256 initialLocation) public returns (bool) {
        uint256 newId = cats.length;
        cats.push(Cat(newId, 0));
        catLocations.push(CatLocation(newId, initialLocation));
        factIDs.push(factInsert cats[newId]);
        factIDs.push(factInsert catLocations[newId]);
        return true;
    }

    function addFood(uint256 location, uint256 energy) public returns (bool) {
        foods.push(Food(location, energy, false));
        factIDs.push(factInsert foods[foods.length-1]);
        return true;
    }

    function queryCatCoord(uint256 catId) public view returns (uint256) {
        assert(catLocations[catId].id == catId);
        return catLocations[catId].value;
    }

    function run() public returns (bool) {
        fireAllRules;
        return true;
    }

    function reset() public returns (bool) {
        for (uint256 i = 0; i < factIDs.length; i++)
            factDelete factIDs[i];
        delete cats;
        delete catLocations;
        delete factIDs;
        return true;
    }

    function () external payable { }
}

