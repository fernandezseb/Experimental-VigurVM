
public class LookupSwitchTest {
    public static void main(String[] args) {
        int fortytwo = 42;
        switch (fortytwo) {
            case 11:
                throw new TestValidationException("Wrong case before");
            case 42:
                break;
            case 22:
                throw new TestValidationException("Wrong case after");
            default:
                throw new TestValidationException("Incorrect fallthrough!");
        }
    }

}