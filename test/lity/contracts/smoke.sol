
contract Smoke {
    uint256[] arr;
    function numberOfElements() public view returns (uint256) {
        return arr.length;
    }
    function addElement(uint256 val) public {
        arr.push(val);
    }
    function getElement(uint i) public view returns (uint256) {
        return arr[i];
    }
}
