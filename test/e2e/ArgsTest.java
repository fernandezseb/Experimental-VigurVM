
public class ArgsTest {
    public static void main(String[] args) {

        if (args.length != 1) {
            throw new TestValidationException("Wrong amount of args");
        }

        if (!args[0].equals("arg1")) {
            throw new TestValidationException("Args is not correct string");
        }


    }
}